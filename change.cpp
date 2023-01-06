//
// Created by chenshein on 1/6/23.
//
smoth{
register int i = m - 2, j; //kernel size always 3 and 3/2 in int is 1
register int index = m * i;
while (i) {
j = m - 2;
while (j) {
dst[index + j] =
applyKernel(i, j, src, kernel, kernelScale, filter
);
--
j;
}
index -=
m;
--
i;
}
}