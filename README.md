# Instagram

The purpose of this project is to take the number of users and the people they follow 
and then show every connected network there is, using C++.

The project uses graph theory concepts such as **Strongly Connected Components**.
These components were found using **The Kosaraju Algorithm"**. **Depth-first search** was performed and
visited vertices were saved in a **B-Tree**. As we can't save strings in a B-tree (only integers),
a **hashing** was needed to convert the given strings into integers.



