% encoding: utf-8
% author: Tegredum
% matlab version: R2024b

% 测试向量归一化方法
disp('测试向量归一化方法...');

% 测试非零向量
testVec = [3; 4];
normalized = LinAlg_teg.normalize(testVec);
assert(abs(norm(normalized) - 1) < eps, '归一化向量范数应为1');
assert(all(abs(normalized - [0.6; 0.8]) < eps), '归一化结果不正确');

% 测试零向量
zeroVec = [0; 0; 0];
normalizedZero = LinAlg_teg.normalize(zeroVec);
assert(all(normalizedZero == 0), '零向量处理不正确');

disp('向量归一化测试通过');

% 测试Householder变换
disp('测试Householder变换...');

% 测试2维向量
vec2d = [1; 2];
[H, v, y] = LinAlg_teg.householderTransform(vec2d);
assert(abs(abs(det(H)) - 1) < 1e-10, 'H矩阵行列式应为±1');
assert(all(all(abs(H*H' - eye(2)) < 1e-10)), 'H矩阵应为正交矩阵');
assert(all(abs(H*vec2d - v) < 1e-10), 'H变换结果不正确');

% 测试3维向量
vec3d = [1; 2; 3];
[H3, v3, y3] = LinAlg_teg.householderTransform(vec3d);
assert(abs(abs(det(H3)) - 1) < eps, 'H矩阵行列式应为±1');
assert(all(all(abs(H3*H3' - eye(3)) < 1e-10)), 'H矩阵应为正交矩阵');
assert(all(abs(H3*vec3d - v3) < eps), 'H变换结果不正确');

disp('Householder变换测试通过');

disp('所有测试通过！');
