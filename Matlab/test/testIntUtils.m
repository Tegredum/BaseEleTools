% encoding: utf-8
% author: Tegredum
% MATLAB version: R2024b
% 一个用于测试 IntUtils 单元的测试程序。
%%
clear
close all

%% 测试 rk4 方法在简单一阶线性微分方程 y' = -2y 上的表现
% 定义微分方程
f = @(t, y) -2 * y;

% 定义初始条件
start_y = 1;
start_time = 0;
end_time = 2;
step_num = 100;

% 使用 rk4 方法进行积分，获取不同积分步数下的结果
y = arrayfun(...
	@(step_num_iter) IntUtils_teg.rk4(...
		'func', f,...
		'start_y', start_y,...
		'start_time', start_time,...
		'end_time', start_time + (end_time - start_time) * (step_num_iter / step_num),...
		'step_num', step_num_iter),...
	1: step_num);

% 计算解析解
t = linspace(start_time, end_time, step_num + 1);
y_exact = start_y * exp(-2 * t);

% 绘制结果
figure
hold on
grid on
plot(t, y_exact)
plot(t, [start_y; y(:)])
xlabel('时间 t')
ylabel('状态 y')
title('4 阶 Runge-Kutta 积分结果与解析解对比')
legend('解析解 y(t)', '数值解 y(t)')

% 计算误差
error = max(abs(y_exact(:) - [start_y; y(:)]));
disp(['最大误差: ', num2str(error)]);

%% 测试 rk4 方法在微分方程返回 [] 时的异常处理能力
% 定义一个故意返回 [] 的微分方程函数
f = @(t, y) [];
start_y = 1;          % 积分起始状态
start_time = 0;       % 积分起始时间
end_time = 2;         % 积分终止时间
step_num = 100;       % 积分步数

% 使用 rk4 方法进行积分
y = IntUtils_teg.rk4(...
	'func', f,...
	'start_y', start_y,...
	'start_time', start_time,...
	'end_time', end_time,...
	'step_num', step_num);
if isempty(y)
	disp('测试通过: rk4 方法正确返回 []')
else
	disp('测试失败: rk4 方法未返回 []')
end

%% 测试 rk4 方法在积分步数为 1 时的表现
% 定义微分方程
f = @(t, y) -2 * y;

% 定义初始条件
start_y = 1;	% 积分起始状态
start_time = 0;	% 积分起始时间
end_time = 0.5;	% 积分终止时间
step_num = 1;	% 积分步数

% 使用 rk4 方法进行积分
y = IntUtils_teg.rk4(...
	'func', f,...
	'start_y', start_y,...
	'start_time', start_time,...
	'end_time', end_time,...
	'step_num', step_num);

% 计算解析解
t = linspace(start_time, end_time, step_num + 1);
y_exact = start_y * exp(-2 * t);

% 绘制结果
figure
hold on
grid on
plot(t, y_exact)
plot(t, [start_y; y(:)])
xlabel('时间 t')
ylabel('状态 y')
title('4 阶 Runge-Kutta 积分结果与解析解对比 (步数为 1)')
legend('解析解 y(t)', '数值解 y(t)')

% 计算误差
error = abs(y_exact(2) - y);
disp(['误差: ', num2str(error)])

%% 测试 rk4 方法在积分起始时间和终止时间相等时的表现
% 定义微分方程
f = @(t, y) -2 * y;

% 定义初始条件
start_y = 1;          % 积分起始状态
start_time = 0;       % 积分起始时间
end_time = 0;         % 积分终止时间
step_num = 100;       % 积分步数（虽然区间为零，步数不影响结果）

% 使用 rk4 方法进行积分
y = IntUtils_teg.rk4(...
	'func', f,...
	'start_y', start_y,...
	'start_time', start_time,...
	'end_time', end_time,...
	'step_num', step_num);

% 预期结果应该是初始状态 start_y
expected_y = start_y;

% 检查结果是否正确
disp(['预期结果与实际结果差值：', num2str(abs(expected_y - y))])

%% 测试 rk4 方法在积分终止时间小于起始时间时的表现

% 定义微分方程
f = @(t, y) -2 * y;

% 定义初始条件
start_y = 1;          % 积分起始状态
start_time = 0;       % 积分起始时间
end_time = -1;        % 积分终止时间（小于起始时间）
step_num = 100;       % 积分步数（尽管区间为负，步数不影响结果）

% 使用 rk4 方法进行积分
y = arrayfun(...
	@(step_num_iter) IntUtils_teg.rk4(...
		'func', f,...
		'start_y', start_y,...
		'start_time', start_time,...
		'end_time', start_time + (end_time - start_time) * (step_num_iter / step_num),...
		'step_num', step_num_iter),...
	1: step_num);

% 计算解析解
t = linspace(start_time, end_time, step_num + 1);
y_exact = start_y * exp(-2 * t);

% 绘制结果
figure
hold on
grid on
plot(t, y_exact)
plot(t, [start_y; y(:)])
xlabel('时间 t')
ylabel('状态 y')
title('4 阶 Runge-Kutta 积分结果与解析解对比（终止时间小于起始时间）')
legend('解析解 y(t)', '数值解 y(t)')

% 计算误差
error = max(abs(y_exact(:) - [start_y; y(:)]));
disp(['最大误差: ', num2str(error)]);

%% 测试不同的积分步数对 rk4 方法积分结果的影响

% 定义微分方程
f = @(t, y) -2 * y;

% 定义初始条件
start_y = 1;          % 积分起始状态
start_time = 0;       % 积分起始时间
end_time = 2;         % 积分终止时间

% 定义不同的积分步数
step_nums = [1, 10, 50, 100, 500, 1000];

% 计算解析解
t_exact = linspace(start_time, end_time, 1000 + 1);
y_exact = start_y * exp(-2 * t_exact);

% 存储不同的积分结果
y_results = cell(length(step_nums), 1);

% 使用不同的步数进行积分
for idx = 1: length(step_nums)
	step_num = step_nums(idx);
	y = arrayfun(...
		@(step_num_iter) IntUtils_teg.rk4(...
			'func', f,...
			'start_y', start_y,...
			'start_time', start_time,...
			'end_time', start_time + (end_time - start_time) * (step_num_iter / step_num),...
			'step_num', step_num_iter),...
		1: step_num);
	y_results{idx} = y;
	disp(['步数为 ', num2str(step_num), ' 时的积分结果: ', num2str(y)])
end

% 绘制结果
figure
hold on
grid on
line_list = [];
line_list(end + 1) = plot(t_exact, y_exact);
lineName_list = {'解析解 y(t)'};
for idx = 1: length(step_nums)
	t = linspace(start_time, end_time, step_nums(idx) + 1);
	line_list(end + 1) = plot(t, [start_y; y_results{idx}(:)]);
    lineName_list{end + 1} = sprintf('步数 = %d', step_nums(idx));
end	
xlabel('时间 t')
ylabel('状态 y')
title('不同步数下的 4 阶 Runge-Kutta 积分结果')
legend(line_list, lineName_list)

% 计算误差
errors = zeros(length(step_nums), 1);
for idx = 1:length(step_nums)
    t = linspace(start_time, end_time, step_nums(idx) + 1);
    y_approx = [start_y; y_results{idx}(:)];
    errors(idx) = max(abs(reshape(start_y * exp(-2 * t), [], 1) - y_approx));
end

% 绘制误差图
figure
hold on
grid on
plot(step_nums, errors, 'o-')
xlabel('积分步数')
ylabel('最大误差')
title('不同步数下的最大误差')

% 显示误差
disp('不同步数下的最大误差:')
disp(errors)

%% 测试 rk4 方法在使用向量作为初始状态时的表现
% 定义微分方程组
f = @(t, y) [-2 * y(1); -1 * y(2)];

% 定义初始条件（向量）
start_y = [1; 1];     % 积分起始状态，向量
start_time = 0;       % 积分起始时间
end_time = 2;         % 积分终止时间
step_num = 100;       % 积分步数

% 使用 rk4 方法进行积分
y = cellfun(...
	@(step_num_iter) IntUtils_teg.rk4(...
		'func', f,...
		'start_y', start_y,...
		'start_time', start_time,...
		'end_time', start_time + (end_time - start_time) * (step_num_iter / step_num),...
		'step_num', step_num_iter),...
	num2cell(1: step_num),...
	'UniformOutput', false);

% 计算解析解
t = linspace(start_time, end_time, step_num + 1);
y_exact_1 = start_y(1) * exp(-2 * t);
y_exact_2 = start_y(2) * exp(-1 * t);
y_exact = [y_exact_1; y_exact_2];

% 绘制结果
figure
hold on
grid on
plot(t, y_exact(1, :))
plot(t, y_exact(2, :))
plot(t, [start_y(1); reshape(...
	cellfun(...
		@(y_per_step) y_per_step(1),...
		y),...
	[], 1)])
plot(t, [start_y(2); reshape(...
	cellfun(...
		@(y_per_step) y_per_step(2),...
		y),...
	[], 1)])
xlabel('时间 t')
ylabel('状态 y')
title('4 阶 Runge-Kutta 积分结果与解析解对比 (向量初始状态)')
legend('解析解 y_1(t)', '解析解 y_2(t)', '数值解 y_1(t)', '数值解 y_2(t)')

% 计算误差
error_1 = max(abs(reshape(y_exact(1, :), [], 1) - [start_y(1); reshape(...
	cellfun(...
		@(y_per_step) y_per_step(1),...
		y),...
	[], 1)]));
error_2 = max(abs(reshape(y_exact(2, :), [], 1) - [start_y(2); reshape(...
	cellfun(...
		@(y_per_step) y_per_step(2),...
		y),...
	[], 1)]));
disp(['最大误差 (y_1): ', num2str(error_1)])
disp(['最大误差 (y_2): ', num2str(error_2)])

%% 测试 rk4 方法在使用矩阵作为初始状态时的表现
% 定义微分方程函数（输入 t 和矩阵 Y，返回导数矩阵 Y）
func_matrix = @(t, Y) Y;  % dY/dt = Y

% 初始矩阵（例如 2x2 矩阵）
Y0 = [1, 2; 
       3, 4];

% 调用 RK4 进行积分（从 t=0 到 t=1，步数 100）
result = IntUtils_teg.rk4(...
    'func', func_matrix, ...
    'start_y', Y0, ...
    'start_time', 0, ...
    'end_time', 1, ...
    'step_num', 100 ...
);

% 解析解（矩阵指数）
analytic_solution = Y0 * exp(1);

% 显示结果对比
disp('数值解：');
disp(result);
disp('解析解：');
disp(analytic_solution);
disp('绝对误差：');
disp(abs(result - analytic_solution));