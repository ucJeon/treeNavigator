# treeNavigator

./treeNavigator <rootfile>

example: 
```
09:31:22  |cicy-py312|ujeon@gate CombineTool ±|main ✗|→ treeNavigator outputs/higgsCombine.Lumi3000.MX10.AsymptoticLimits.mH1000.root
Available Trees:
  limit

Enter the tree name (or '.q' to quit):
```
limit is corresponding to treeName.

enter the treeName:
```
Available Trees:
  limit

Enter the tree name (or '.q' to quit): limit

Options for tree "limit":
1: Print branches again
2: Print events
3: Scan root file
4: ScanMode(It's temporary for class)
0: Return to tree selection
Enter your choice:
```
option
- 1 : just print branches in the tree
- 2 : you can print-out contents with entry number (index, 0 is first entry).
- 3 : is not used.
- 4 : if you enter the branch, PDF file is generated in your current directory.

> If you are FALL INTO LOOP HELL -> (control + c) for Macbook, iMac, ect
