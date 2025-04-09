% Test program for Poly_teg class
% This script tests all functionality of the Poly_teg polynomial class

% Clear workspace
clear all;
close all;
clc;

%% Test 1: Constructor and basic properties
fprintf('=== Test 1: Constructor and basic properties ===\n');
p1 = Poly_teg(3, [1 2 3]); % 1 + 2x + 3x^2
fprintf('Created polynomial p1 = 1 + 2x + 3x^2\n');
fprintf('MaxDegree: %d (Expected: 3)\n', p1.maxDegree);
fprintf('ActualDegree: %d (Expected: 2)\n', p1.actualDegree);
fprintf('Coefficients: ');
disp(p1.coeffs);
assert(p1.maxDegree == 3, 'MaxDegree test failed');
assert(p1.actualDegree == 2, 'ActualDegree test failed');
assert(isequal(p1.coeffs, [1 2 3 0]), 'Coefficients test failed');
fprintf('Test 1 passed\n\n');

%% Test 2: Coefficient access
fprintf('=== Test 2: Coefficient access ===\n');
fprintf('Coefficient at degree 0: %g (Expected: 1)\n', p1.getCoefficient(0));
fprintf('Coefficient at degree 2: %g (Expected: 3)\n', p1.getCoefficient(2));
assert(p1.getCoefficient(0) == 1, 'Degree 0 coefficient test failed');
assert(p1.getCoefficient(2) == 3, 'Degree 2 coefficient test failed');
fprintf('Test 2 passed\n\n');

%% Test 3: Polynomial addition
fprintf('=== Test 3: Polynomial addition ===\n');
p2 = Poly_teg(2, [4 5]); % 4 + 5x
p3 = p1 + p2; % (1 + 2x + 3x^2) + (4 + 5x) = 5 + 7x + 3x^2
fprintf('p1 + p2 = ');
disp(p3.coeffs);
assert(isequal(p3.coeffs, [5 7 3 0]), 'Polynomial addition test failed');
fprintf('Test 3 passed\n\n');

%% Test 4: Polynomial multiplication
fprintf('=== Test 4: Polynomial multiplication ===\n');
p4 = p1 * p2; % (1 + 2x + 3x^2) * (4 + 5x) = 4 + 13x + 22x^2 + 15x^3
fprintf('p1 * p2 = ');
disp(p4.coeffs);
assert(isequal(p4.coeffs(1: 4), [4 13 22 15]), 'Polynomial multiplication test failed');
fprintf('Test 4 passed\n\n');

%% Test 5: Scalar multiplication
fprintf('=== Test 5: Scalar multiplication ===\n');
p5 = p1 * 2.0; % 2*(1 + 2x + 3x^2) = 2 + 4x + 6x^2
fprintf('2 * p1 = ');
disp(p5.coeffs);
assert(isequal(p5.coeffs, [2 4 6 0]), 'Scalar multiplication test failed');
fprintf('Test 5 passed\n\n');

%% Test 6: Derivative
fprintf('=== Test 6: Derivative ===\n');
dp1 = p1.derivative(); % d/dx(1 + 2x + 3x^2) = 2 + 6x
fprintf('Derivative of p1 = ');
disp(dp1.coeffs);
assert(isequal(dp1.coeffs, [2 6 0]), 'Derivative test failed');
fprintf('Test 6 passed\n\n');

%% Test 7: Evaluation
fprintf('=== Test 7: Evaluation ===\n');
x = 2;
val = p1.evaluate(x); % 1 + 2*2 + 3*4 = 17
fprintf('p1 evaluated at x=%g: %g (Expected: 17)\n', x, val);
assert(val == 17, 'Evaluation test failed');
fprintf('Test 7 passed\n\n');

%% Test 8: Clear coefficients
fprintf('=== Test 8: Clear coefficients ===\n');
p1.clearCoefficients();
fprintf('After clear, coefficients: ');
disp(p1.coeffs);
assert(isequal(p1.coeffs, [0 0 0 0]), 'Clear coefficients test failed');
assert(p1.actualDegree == 0, 'ActualDegree after clear test failed');
fprintf('Test 8 passed\n\n');

%% Summary
fprintf('=== All tests passed ===\n');
fprintf('Poly_teg class implementation appears correct.\n');
