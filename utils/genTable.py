output = open("Connection_Table_temp.txt","w+")
for i in range(0,1023+1):
    for j in range(0,1023+1):
        if i != j:
            str="%d %d 2 100\n" %(i,j)
            output.write(str)
