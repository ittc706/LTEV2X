[data1,data2] = textread('Sprinkle_URBAN.txt','%n%n');
dim = length(data1);
plot(data1,data2,'x');