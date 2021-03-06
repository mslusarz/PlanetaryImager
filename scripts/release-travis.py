#!/usr/bin/env python
from github import Github
from github.GithubException import UnknownObjectException
import os
import sys
import traceback
import time

github = Github(os.environ['GITHUB_OAUTH_USER'], os.environ['GITHUB_OAUTH_TOKEN'])
repo = github.get_repo('GuLinux/PlanetaryImager')

release_name = os.environ['PLANETARY_IMAGER_VERSION']
release_tag = 'v' + release_name
release_body = os.environ['TRAVIS_COMMIT_MESSAGE']
commit_id = os.environ['TRAVIS_COMMIT']

pr = None

pr_number = os.environ.get('TRAVIS_PULL_REQUEST', 'false')
if pr_number and pr_number != 'false':
    try:
        pr = repo.get_pull(int(pr_number))
    except ValueError:
        pass


def find_pr():
    all_pulls = [pr for pr in repo.get_pulls(state='closed', sort='updated')]
    all_pulls.reverse()
    for merged_pr in all_pulls:
        if merged_pr.merge_commit_sha == commit_id:
            return merged_pr
    return None

if not pr:
    retry = 0
    while not pr and retry < 5:
        time.sleep(retry * 10)
        try:
            pr = find_pr()
        except:
            traceback.print_exc()
            retry += 1

if pr:
    release_body='''## {}  
{}

### Commit message:
```
{}
```
'''.format(pr.title, pr.body, release_body)

release = None
try:
    release = repo.get_release(release_tag)
except UnknownObjectException:
    release = repo.create_git_release(release_tag, release_name, release_body, draft=False, prerelease=True, target_commitish=commit_id)

print('Release created/updated: ' + release_tag)
for asset in sys.argv[1:]:
    print(' - deploying asset: ' + asset)
    release.upload_asset(asset)
