
fileid = fopen('vector.txt', 'r');

[a,n] = fscanf(fileid, ['%f'])

avg = mean(a)
probplot(a)