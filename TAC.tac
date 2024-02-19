1. out num
2. out "\n"
3. out num
4. out "\n"
5. out num
6. out "\n"
7. out num
8. out "\n"
9. out num
10. out "\n"
11. i = 0
12. out "\n"
13. ret 0
14. out "oh"
15. out "Enter the number"
16. in n
17. out n
18. out " "
19. param 1
20. param 2
21. call calculateFib, 2, t1
22. pa = t1
23. out pa
24. if n = 1 goto 26
25. goto 27
26. out "aik daal diya\n"
27. if n = 0 goto 29
28. goto 30
29. out "yar 0 hai!\n"
30. out "\nhan ab sahi hay: "
31. ret 0
