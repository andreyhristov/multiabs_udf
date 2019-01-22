# MULTI_ABS() UDF function

This is a MySQL UDF which does multiple ABS() calls to pairs of the arguments and accumulates the result.

 


## Examples
MULTI_ABS() returns 0
MULTI_ABS(3, 8) returns 5
MULTI_ABS(3, 8, 12, 25) returns ABS(3,8)+ABS(12,25) = 18

MULTI_ABS(3) will error because the argument count is odd
MULTI_ABS(3,5,8) will error because the argument count is odd