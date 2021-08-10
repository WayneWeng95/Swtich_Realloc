import random
 
num = 1
while num < 1000:
    password = random.randint(1024,2145000000)
    txt= str(password) + "\n"
    num +=1
    with open("rand7.txt","a") as f:
        f.write(txt)
