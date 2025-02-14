% encoding: utf-8
% author: Tegredum
% MATLAB version: R2024b
% 一些导弹制导过程中关注的某些关键量（比如剩余飞行时间和 ZEM 矢量）及其偏导数的计算方法，以及某些简单制导律的实施程序
classdef GuidUtils_teg
	methods(Static)
		% 计算终端指征 YT，说明详见文档
		function YT = calc_YT(inArgs)
			arguments(Input)
				inArgs.rm	(3, 1)	double	% 导弹位置矢量
				inArgs.vm	(3, 1)	double	% 导弹速度矢量
				inArgs.rt	(3, 1)	double	% 目标位置矢量
				inArgs.vt	(3, 1)	double	% 目标速度矢量
			end
			arguments(Output)
				YT	(1, 1)	double	% 终端指征
			end
			YT = dot(inArgs.rt - inArgs.rm, inArgs.vt - inArgs.vm);
		end
		% 计算终端指征关于位置矢量的偏导数
		function result = par_YT_par_r(inArgs)
			arguments(Input)
				inArgs.vm		(3, 1)	double	% 导弹速度矢量
				inArgs.vt		(3, 1)	double	% 目标速度矢量
				inArgs.obj_sel	char			% 选择需要计算位置偏导数的对象
			end
			arguments(Output)
				result	(1, 3)	double	% 终端指征关于位置矢量的偏导数
			end
			vm = inArgs.vm;
			vt = inArgs.vt;
			if strcmp(inArgs.obj_sel, 'missile')
				result = -(vt - vm)';
			elseif strcmp(inArgs.obj_sel, 'target')
				result = (vt - vm)';
			else
				error('Invalid object selection')
			end
		end
		% 计算终端指征关于速度矢量的偏导数
		function result = par_YT_par_v(inArgs)
			arguments(Input)
				inArgs.rm		(3, 1)	double	% 导弹位置矢量
				inArgs.rt		(3, 1)	double	% 目标位置矢量
				inArgs.obj_sel	char			% 选择需要计算速度偏导数的对象
			end
			arguments(Output)
				result	(1, 3)	double	% 终端指征关于速度矢量的偏导数
			end
			rm = inArgs.rm;
			rt = inArgs.rt;
			if strcmp(inArgs.obj_sel, 'missile')
				result = -(rt - rm)';
			elseif strcmp(inArgs.obj_sel, 'target')
				result = (rt - rm)';
			else
				error('Invalid object selection')
			end
		end
		% 估计剩余飞行时间
		function tgo = calc_tgo(inArgs)
			arguments(Input)
				inArgs.rm	(3, 1)	double	% 导弹位置矢量
				inArgs.vm	(3, 1)	double	% 导弹速度矢量
				inArgs.rt	(3, 1)	double	% 目标位置矢量
				inArgs.vt	(3, 1)	double	% 目标速度矢量
			end
			arguments(Output)
				tgo	(1, 1)	double	% 剩余飞行时间
			end
			vm = inArgs.vm;
			vt = inArgs.vt;
			% 计算公式详见说明文档
			YT = GuidUtils_teg.calc_YT(...
				'rm', inArgs.rm,...
				'vm', vm,...
				'rt', inArgs.rt,...
				'vt', vt);
			delta_v = vt - vm;
			tgo = -YT / dot(delta_v, delta_v);
		end
		% 计算剩余飞行时间关于位置矢量的偏导数
		function result = par_tgo_par_r(inArgs)
			arguments(Input)
				inArgs.vm		(3, 1)	double	% 导弹速度矢量
				inArgs.vt		(3, 1)	double	% 目标速度矢量
				inArgs.obj_sel	char			% 选择需要计算位置偏导数的对象
			end
			arguments(Output)
				result	(1, 3)	double	% 剩余飞行时间关于位置矢量的偏导数
			end
			vm = inArgs.vm;
			vt = inArgs.vt;
			% 计算公式详见说明文档
			delta_v = vt - vm;
			result = -GuidUtils_teg.par_YT_par_r(...
				'vm', vm,...
				'vt', vt,...
				'obj_sel', inArgs.obj_sel) / dot(delta_v, delta_v);
		end
		% 计算剩余飞行时间关于速度矢量的偏导数
		function result = par_tgo_par_v(inArgs)
			arguments(Input)
				inArgs.rm		(3, 1)	double	% 导弹位置矢量
				inArgs.vm		(3, 1)	double	% 导弹速度矢量
				inArgs.rt		(3, 1)	double	% 目标位置矢量
				inArgs.vt		(3, 1)	double	% 目标速度矢量
				inArgs.obj_sel	char			% 选择需要计算速度偏导数的对象
			end
			arguments(Output)
				result	(1, 3)	double	% 剩余飞行时间关于速度矢量的偏导数
			end
			rm = inArgs.rm;
			vm = inArgs.vm;
			rt = inArgs.rt;
			vt = inArgs.vt;
			% 计算公式详见说明文档
			delta_v = vt - vm;
			YT = GuidUtils_teg.calc_YT(...
				'rm', rm,...
				'vm', vm,...
				'rt', rt,...
				'vt', vt);
			mat_par_YT_par_v = GuidUtils_teg.par_YT_par_v(...
				'rm', rm,...
				'rt', rt,...
				'obj_sel', inArgs.obj_sel);
			if strcmp(inArgs.obj_sel, 'missile')
				par_inv_delta_v_par_v = delta_v' * (2 / dot(delta_v, delta_v)^2);
			elseif strcmp(inArgs.obj_sel, 'target')
				par_inv_delta_v_par_v = delta_v' * (-2 / dot(delta_v, delta_v)^2);
			else
				error('Invalid object selection')
			end
			result = mat_par_YT_par_v / (-dot(delta_v, delta_v)) - par_inv_delta_v_par_v * YT;
		end
		% 估计 ZEM 矢量
		function zem = calc_zem(inArgs)
			arguments(Input)
				inArgs.rm	(3, 1)	double	% 导弹位置矢量
				inArgs.vm	(3, 1)	double	% 导弹速度矢量
				inArgs.rt	(3, 1)	double	% 目标位置矢量
				inArgs.vt	(3, 1)	double	% 目标速度矢量
			end
			arguments(Output)
				zem	(3, 1)	double	% ZEM 矢量
			end
			rm = inArgs.rm;
			vm = inArgs.vm;
			rt = inArgs.rt;
			vt = inArgs.vt;
			tgo = GuidUtils_teg.calc_tgo(...	% 计算剩余飞行时间
				'rm', rm,...
				'vm', vm,...
				'rt', rt,...
				'vt', vt);
			zem = rt - rm + (vt - vm) * tgo;
		end
		% 计算 ZEM 矢量关于位置矢量的偏导数
		function result = par_zem_par_r(inArgs)
			arguments(Input)
				inArgs.vm		(3, 1)	double	% 导弹速度矢量
				inArgs.vt		(3, 1)	double	% 目标速度矢量
				inArgs.obj_sel	char			% 选择需要计算位置偏导数的对象
			end
			arguments(Output)
				result	(1, 3)	double	% ZEM 矢量关于位置矢量的偏导数
			end
			vm = inArgs.vm;
			vt = inArgs.vt;
			% 计算公式详见说明文档
			delta_v = vt - vm;
			mat_par_tgo_par_r = GuidUtils_teg.par_tgo_par_r(...
				'vm', vm,...
				'vt', vt,...
				'obj_sel', inArgs.obj_sel);
			if strcmp(inArgs.obj_sel, 'missile')
				result = -eye(3) + delta_v * mat_par_tgo_par_r;
			elseif strcmp(inArgs.obj_sel, 'target')
				result = eye(3) + delta_v * mat_par_tgo_par_r;
			else
				error('Invalid object selection')
			end
		end
		% 计算 ZEM 矢量关于速度矢量的偏导数
		function result = par_zem_par_v(inArgs)
			arguments(Input)
				inArgs.rm		(3, 1)	double	% 导弹位置矢量
				inArgs.vm		(3, 1)	double	% 导弹速度矢量
				inArgs.rt		(3, 1)	double	% 目标位置矢量
				inArgs.vt		(3, 1)	double	% 目标速度矢量
				inArgs.obj_sel	char			% 选择需要计算速度偏导数的对象
			end
			arguments(Output)
				result	(1, 3)	double	% ZEM 矢量关于速度矢量的偏导数
			end
			rm = inArgs.rm;
			vm = inArgs.vm;
			rt = inArgs.rt;
			vt = inArgs.vt;
			% 计算公式详见说明文档
			delta_v = vt - vm;
			tgo = GuidUtils_teg.calc_tgo(...	% 计算剩余飞行时间
				'rm', rm,...
				'vm', vm,...
				'rt', rt,...
				'vt', vt);
			mat_par_tgo_par_v = GuidUtils_teg.par_tgo_par_v(...
				'rm', rm,...
				'vm', vm,...
				'rt', rt,...
				'vt', vt,...
				'obj_sel', inArgs.obj_sel);
			if strcmp(inArgs.obj_sel, 'missile')
				result = eye(3) * (-tgo) + delta_v * mat_par_tgo_par_v;
			elseif strcmp(inArgs.obj_sel, 'target')
				result = eye(3) * tgo + delta_v * mat_par_tgo_par_v;
			else
				error('Invalid object selection')
			end
		end
	end
end