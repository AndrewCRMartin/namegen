Obtaining full name list
========================

- Log in to mednet
- Go to the INN search page
- search for 'mab' in the middle of a name to obtain a full list of
  all entries containing 'mab'. Ones that don't have -mab as a step
  will be filtered later.
- Save the page as 'HTML only'
- Update the names data by running:
```
   ./updatenames.sh "INN Search.html"
```
- This will create the full CSV file of names plus the simple names list (with extensions removed) and add this to git/github


