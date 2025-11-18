syms a1 a2 a3

mtx123 = euler2matrix([a1,a2,a3],[1,2,3]);
mtx231 = euler2matrix([a1,a2,a3],[2,3,1]);
mtx312 = euler2matrix([a1,a2,a3],[3,1,2]);

mtx321 = euler2matrix([a1,a2,a3],[3,2,1]);
mtx213 = euler2matrix([a1,a2,a3],[2,1,3]);
mtx132 = euler2matrix([a1,a2,a3],[1,3,2]);

mtx121 = euler2matrix([a1,a2,a3],[1,2,1]);
mtx232 = euler2matrix([a1,a2,a3],[2,3,2]);
mtx313 = euler2matrix([a1,a2,a3],[3,1,3]);

mtx131 = euler2matrix([a1,a2,a3],[1,3,1]);
mtx212 = euler2matrix([a1,a2,a3],[2,1,2]);
mtx323 = euler2matrix([a1,a2,a3],[3,2,3]);

function [mtx] = euler2matrix(angles, seq)
    mtx = [1,0,0;0,1,0;0,0,1];
    for i=1:length(angles)
        mtx = rot(angles(i), seq(i)) * mtx;
    end
end

function [mtx] = rot(angle, axis)
c = cos(angle);
s = sin(angle);
if axis == 1
    mtx = [
        1, 0, 0;
        0, c, s;
        0, -s,c
    ];
elseif axis == 2
    mtx = [
        c, 0, -s;
        0, 1, 0;
        s, 0, c
    ];
elseif axis == 3
    mtx = [
      c, s, 0;
      -s, c, 0;
      0, 0, 1
    ];
end    
end