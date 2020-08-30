#!/usr/bin/python
# vv

import os
import sys
import unittest
import errno
import pexpect
from random import randrange
from time import time, sleep

#
# Globals
#
ignores_sigterm = './ignore_sigterm'
exe_path = './smash'

PROMPT = 'smash > '
PROMPT_ERROR = 'smash error: > '

def is_running(pid):        
    try:
        os.kill(pid, 0)
    except OSError as err:
        if err.errno == errno.ESRCH:
            return False
    return True

def parse_jobs(s):
    # [1] sleep 100 : 110206 10 secs
    jobs = []
    lines = s.splitlines()
    lines = [x.strip() for x in lines if x.strip()]
    # only lines starting with [ (ignore other prints)
    lines = filter(lambda x: x.startswith('['), lines)
    get_pid = lambda l: int(l.split(':')[-1].split()[0])
    get_cmd = lambda l: l.split(':')[0].split(']')[-1].strip()
    jobs = [{'pid': get_pid(l), 'cmd': get_cmd(l)} for l in lines]
    return jobs
    
class TestsBase(unittest.TestCase):
    def setUp(self):
        # spawn smash
        self.p = pexpect.spawn('./smash', timeout=1)
        # disable tty echo
        self.p.setecho(False)
        # wait for first prompt
        self.p.expect(PROMPT)
        # smash spawned job pids
        self.jobs = []
    def tearDown(self):
        if not self.p.isalive():
            return
        # kill it
        self.p.sendline('quit kill')
        # timeout should be number of jobs * 5 + 1
        try:
            self.p.expect(pexpect.EOF, timeout=1 + len(self.jobs)*5)
        finally:
            self.p.terminate(True)

class CmdTests(TestsBase):
    def _get_nonexisting_path(self, base='/tmp', prefix=os.getenv('USER'), tries=100):
        for _ in range(tries):
            d = os.path.join(base, prefix + str(randrange(2**30)))
            if not os.path.exists(d):
                return d
        raise RuntimeError("Could not find a nonexisting path")
    def test_pwd(self):
        self.p.sendline('pwd')
        self.p.expect(PROMPT)
        self.assertEqual(self.p.before.rstrip(), os.getcwd())
    def test_cd(self):
        d = '/tmp'
        orig_cd = os.getcwd()
        # test cd <dir>
        self.p.sendline('cd %s' % d)
        self.p.expect(PROMPT)
        self.p.sendline('pwd')
        self.p.expect(PROMPT)
        self.assertEqual(self.p.before.rstrip(), d)
        # test cd -
        self.p.sendline('cd -')
        self.p.expect(PROMPT)
        self.p.sendline('pwd')
        self.p.expect(PROMPT)
        self.assertEqual(self.p.before.rstrip(), orig_cd)
        
        # test cd <nonexisting dir>
        nopath = self._get_nonexisting_path()
        self.p.sendline('cd %s' % nopath)
        self.p.expect(PROMPT)
        expect = PROMPT_ERROR + '"%s" - path not found' % nopath
        self.assertEqual(self.p.before.rstrip(), expect)

    def test_mkdir(self):
        d = self._get_nonexisting_path()
        # mkdir <dir>
        self.p.sendline('mkdir %s' % d)
        self.p.expect(PROMPT)

        # test if dir exists
        self.assertTrue(os.path.isdir(d), 'sent mkdir %s cmd, but dir doesn\'t exist' % d)
        # if exists, delete it
        os.rmdir(d)

        # mkdir <existing dir>
        self.p.sendline('mkdir /tmp')
        expect = PROMPT_ERROR + '"/tmp" - directory already exists'
        self.p.expect(PROMPT)
        self.assertEqual(self.p.before.rstrip(), expect)

        # mkdir <no permission dir>
        d = '/bsfasdfasdfasdf'
        self.p.sendline('mkdir %s' % d)
        expect = PROMPT_ERROR + '"%s" - cannot create directory' % d
        self.p.expect(PROMPT)
        self.assertEqual(self.p.before.rstrip(), expect)

    # jobs
    def test_jobs(self):
        # spawn some jobs
        num_jobs = 8
        sleep_time = 2
        for i in range(num_jobs):
            self.p.sendline('sleep %d&' % sleep_time)
            self.p.expect(PROMPT)
        self.p.sendline('jobs')
        self.p.expect(PROMPT)
        jobs = parse_jobs(self.p.before)
        self.assertEqual(len(jobs), num_jobs)
        self.assertEqual(set([job['cmd'].split()[0] for job in jobs]), set(['sleep']))
        # wait for jobs to finish
        sleep(sleep_time+1)
        self.p.sendline('jobs')
        self.p.expect(PROMPT)
        jobs = parse_jobs(self.p.before)
        # we should have no jobs
        self.assertEqual(len(jobs), 0)

    # kill
    def test_showpid(self):
        self.p.sendline('showpid')
        self.p.expect(PROMPT)
        self.assertEqual(self.p.before.rstrip(),
                        'smash pid is %d' % self.p.pid)
    # fg
    # bg
    # quit ?
    
class ExeTests(TestsBase):
    def test_external(self):
        self.p.sendline('ls /dev/null')
        self.p.expect(PROMPT)
        self.assertEqual(self.p.before.rstrip(),
                        '/dev/null')
    def test_complicated(self):
        self.p.sendline('ls /dev | grep -E ^null$')
        self.p.expect(PROMPT)
        self.assertEqual(self.p.before.rstrip(),
                        'null')
    def test_background(self):
        start = time()
        sleep_time = 2
        epsilon = 1 # we will wait sleep_time + epsilon for completion.
        self.p.sendline('sleep %d&' % sleep_time)
        # wait for prompt (should take long)
        try:
            self.p.expect(PROMPT, timeout=0.5)
        except:
            raise AssertionError("background command sent but shell did not return")
        
        # make sure task is running (using built in jobs cmd :))
        self.p.sendline('jobs')
        self.p.expect(PROMPT)
        jobs = parse_jobs(self.p.before)
        pid, cmd = jobs[0]['pid'], jobs[0]['cmd']
        self.assertEqual(len(jobs), 1)
        # read /proc file system
        pid_proc = os.path.join('/proc', str(pid))
        self.assertTrue(os.path.exists(pid_proc))
        # OK, /proc/pid exists.. check process name
        expected_cmd = open(os.path.join('/proc', str(pid), 'cmdline'), 'r').read()
        # compare beginning of cmd
        self.assertEqual(cmd.split()[0], expected_cmd.split('\0')[0])
        
        while is_running(pid):
            sleep(0.1)
            if time()-start > sleep_time+epsilon:
                raise AssertionError("spawned bg process (sleep) ran too long")
        # ok :)
        # make sure it's not in jobs list anymore
        self.p.sendline('jobs')
        self.p.expect(PROMPT)
        jobs = parse_jobs(self.p.before)
        self.assertEqual(len(jobs), 0)

class BadCmdTests(TestsBase):
    def _test_bad_cmd(self, cmd):
        self.p.sendline(cmd)
        self.assertEqual(self.p.readline().rstrip(),
                    'smash error: > "%s"' % cmd)
    def test_bad_pwd(self):
        self._test_bad_cmd('pwd bah')
    def test_bad_cd(self):
        self._test_bad_cmd('cd')
    def test_bad_mkdir(self):
        self._test_bad_cmd('mkdir')
    def test_bad_jobs(self):
        self._test_bad_cmd('jobs lolz')
    def test_bad_kill(self):
        self._test_bad_cmd('kill --')
    def test_bad_showpid(self):
        self._test_bad_cmd('showpid lolz')
    def test_bad_fg(self):
        self._test_bad_cmd('fg --')
    def test_bad_bg(self):
        self._test_bad_cmd('bg --')
    def test_bad_quit(self):
        self._test_bad_cmd('quit nokillhaha')
    
if __name__ == '__main__':
    unittest.main()
