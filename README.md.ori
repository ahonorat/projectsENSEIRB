# projectsENSEIRB

Various projects when studying at ENSEIRB-MATMECA.

## Projects

See corresponding subfolder.
- CouchBase
- 3-5-6TDP
- 4TDP
- usersThreads
- compilationI2


## Merging processus

Adapted from https://medium.com/@leyanlo/how-to-move-one-git-repository-into-a-subdirectory-of-another-with-rebase-2b297b628c57

```
git clone old-project
cd old-project
mkdir subdir
git mv !(old-project) subdir
git ci -am "subfolder to merge into bigger repo"
git push
FIRST_COMMIT_HASH=$(git rev-list HEAD | tail -n 1)
git filter-branch -f --msg-filter 'sed "1 s/^/[subdir] /"' ${FIRST_COMMIT_HASH}..HEAD
git rebase -i --root # amend first commit
git push origin HEAD:branchName

cd ../new-project
git remote add side_proj https://github.com/user/side_proj.git
git fetch side_proj
git checkout remotes/side_proj/branchName
git rebase master
git pull --allow-unrelated-histories
git push
git remote rm side_proj
```