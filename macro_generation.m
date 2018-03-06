u = 1.6*[sin(linspace(0,2*pi)), mod(i,2)*sin(linspace(2*pi,0))];
v = 1.6*[mod(i,2)*sin(linspace(0,2*pi)), sin(linspace(2*pi,0))];
out = [];
for i = 0:4
    joint = zeros(1,200)+i;
    out = [out;[joint',v',u']];
end