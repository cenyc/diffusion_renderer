from enoki import *
from enoki.cuda_autodiff import Float32 as FloatD, Vector3f as Vector3D

a = normalize(Vector3D(2, 1, 3))
b = normalize(Vector3D(-1, 2, 3))
