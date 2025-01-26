% encoding: utf-8
% author: Tegredum
% MATLAB version: R2024b
% 提供若干常用积分算法接口
classdef IntUtils_teg
	methods (Static)
		% 4 阶 Runge-Kutta 法，若积分过程出现异常，返回 []
		function y = rk4(inArgs)
			arguments
				inArgs.func			function_handle		% 待积分系统方程，形式为 y' = f(t, y)，若出现异常，返回 []
				inArgs.start_y		double				% 积分起始状态，可以为标量、矢量矩阵或者别的可以加减数乘的数据结构
				inArgs.start_time	(1, 1)	double		% 积分起始时间
				inArgs.end_time		(1, 1)	double		% 积分终止时间
				inArgs.step_num		(1, 1)	double = 1	% 积分步数
			end
			f = inArgs.func;
			t = inArgs.start_time;
			y = inArgs.start_y;
			h = (inArgs.end_time - inArgs.start_time) / inArgs.step_num;	% 这里我们认为 inArgs.step_num 一般不会为零，就不额外检查了
			for idx = 1: inArgs.step_num
				k1 = f(t, y) * h;
				if isempty(k1)
					y = [];
					return
				end
				k2 = f(t + h / 2, y + k1 / 2) * h;
				if isempty(k2)
					y = [];
					return
				end
				k3 = f(t + h / 2, y + k2 / 2) * h;
				if isempty(k3)
					y = [];
					return
				end
				k4 = f(t + h, y + k3) * h;
				if isempty(k4)
					y = [];
					return
				end
				y = y + (k1 + k2 * 2 + k3 * 2 + k4) / 6;
				t = t + h;
			end
		end
	end
end