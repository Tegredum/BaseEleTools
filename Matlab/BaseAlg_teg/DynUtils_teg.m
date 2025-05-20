% encoding: utf-8
% author: Tegredum
% MATLAB version: R2024b
% 一些动力学相关计算的工具函数
classdef DynUtils_teg
	methods
		% 基于相对距离、视线角、弹目速度矢量计算相对距离、视线角变化率
		function outArgs = calc_drq_by_rqAndVel(inArgs)
			arguments
				inArgs.rq			(3, 1)	double	% 相对距离与视线角构成的矢量 [r; qe; qb]
				inArgs.vel_missile	(3, 1)	double	% 导弹速度矢量
				inArgs.vel_target	(3, 1)	double	% 目标速度矢量
			end
			outArgs = struct();
			% 原理详见文档
			midMatInv_outArgs = TransUtils_teg.par_vec_par_rtp_inv(inArgs.rq); %#ok<*MCNPN>
			outArgs.drq = midMatInv_outArgs.result * (inArgs.vel_target - inArgs.vel_missile);
			outArgs.status = midMatInv_outArgs.status;
		end
		% 输入水平视线角，输出水平向相对距离关于弹目相对位置矢量的偏导数矩阵
		function result = calc_par_drh_par_relVel(qb)
			arguments
				qb	(1, 1)	double	% 水平视线角
			end
			result = [cos(qb), 0.0, -sin(qb)];	% 公式详见文档
		end
		% 基于水平视线角、弹目速度矢量计算水平向相对距离变化率
		function result = calc_drh_by_qbAndVel(inArgs)
			arguments
				inArgs.qb			(1, 1)	double	% 水平视线角
				inArgs.vel_missile	(3, 1)	double	% 导弹速度矢量
				inArgs.vel_target	(3, 1)	double	% 目标速度矢量
			end
			result = DynUtils_teg.calc_par_drh_par_relVel(inArgs.qb) * (inArgs.vel_target - inArgs.vel_missile);	% 公式详见文档
		end
		% 计算相对距离、水平相对距离、视线角变化率关于相对速度矢量的偏导数矩阵
		function outArgs = calc_par_drrhq_par_relVel(rhhq)
			arguments
				rhhq	(4, 1)	double	% 相对距离、水平相对距离、视线角变化率构成的矢量 [r; rh; qe; qb]
			end
			outArgs = struct();
			result = zeros(4 ,3);	% 偏导数矩阵
			% 原理详见文档
			r = rhhq(1);
			qe = rhhq(3);
			qb = rhhq(4);
			rq = [r, qe, qb];
			midInvMatArgs = TransUtils_teg.par_vec_par_rtp_inv(rq);
			outArgs.status = midInvMatArgs.status;
			result(1, :) = midInvMatArgs.result(1, :);
			result(3: 4, :) = midInvMatArgs.result(2: 3, :);
			result(2, :) = DynUtils_teg.calc_par_drh_par_relVel(qb);
			outArgs.result = result;
		end
		% 计算相对距离、水平相对距离、视线角变化率关于相对距离、水平相对距离、视线角的偏导数矩阵（针对抑制动力学非线性而增加了水平相对距离状态量的情形）
		function outArgs = calc_par_drrhq_par_rhhq(inArgs)
			arguments
				inArgs.rhhq			(4, 1)	double	% 相对距离、水平相对距离、视线角变化率构成的矢量 [r; rh; qe; qb]
				inArgs.vel_missile	(3, 1)	double	% 导弹速度矢量
				inArgs.vel_target	(3, 1)	double	% 目标速度矢量
			end
			outArgs = struct();
			result = zeros(4 ,4);	% 偏导数矩阵
			status = 'ok';
			delta_vel = inArgs.vel_target - inArgs.vel_missile;	% 相对速度矢量
			% 原理详见文档
			r = inArgs.rhhq(1);
			rh = inArgs.rhhq(2);
			qe = inArgs.rhhq(3);
			qb = inArgs.rhhq(4);
			rq = [r, qe, qb];
			midInvParMat_array = cell(3, 1);	% 文档中那些极为复杂的偏导数逆矩阵对标量偏导数矩阵都存在这个数组里。从前到后分别是对 r、qe、qb 的偏导数矩阵
			for idx = 1: 3
				midArgs = TransUtils_teg.par_par_vec_par_rtp_inv_byIdx(rq, idx - 1);
				% 异常标志字
				if ~strcmp(midArgs.status, 'ok')
					if strcmp(status, 'ok')
						status = midArgs.status;
					else
						if strcmp(status, 'vertical') && strcmp(midArgs.status, 'zeroR')
							status = midArgs.status;
						end
					end
				end
				midInvParMat_array{idx} = midArgs.result;
			end
			rowIdx_mapArray = [1, 3];
			colIdx_mapArray = [1, 3, 4];
			for rowIdx = 1: 2
				for colIdx = 1: 3
					result(rowIdx_mapArray(rowIdx), colIdx_mapArray(colIdx)) = midInvParMat_array{colIdx}(rowIdx, :) * delta_vel;
				end
			end
			sqb = sin(qb);
			cqb = cos(qb);
			result(2, 4) = -dot([sqb, 0.0, cqb], delta_vel);
			if rh^2 == 0.0
				if strcmp(status, 'ok')
					outArgs.status = 'vertical';
				end
				outArgs.result = result;
				return
			end
			result(4, 2) = dot([sqb, 0.0, cqb], delta_vel) / rh^2;
			result(4, 4) = -dot([cqb, 0.0, sqb], delta_vel) / rh;
			outArgs.status = status;
			outArgs.result = result;
		end
		% 计算速度大小、速度方向角变化率
		function outArgs = calc_dvgp(inArgs)
			arguments
				inArgs.a	(3, 1)	double	% 弹道系加速度矢量 [ax; ay; az]
				inArgs.vgp	(3, 1)	double	% [V; gamma; psi]
				inArgs.g	(1, 1)	double	% 重力加速度
			end
			outArgs = struct();
			outArgs.status = 'ok';
			% 原理详见文档
			ax = inArgs.a(1);
			ay = inArgs.a(2);
			az = inArgs.a(3);
			V = inArgs.vgp(1);
			gamma = inArgs.vgp(2);
			cgamma = cos(gamma);
			g = inArgs.g;
			outArgs.result = [
				ax - g * cgamma;
				zeros(2, 1)
			];
			if V == 0.0
				outArgs.status = 'zeroR';
				return
			end
			outArgs.result(2) = (ay - g * cgamma) / V;
			if V * cgamma == 0.0
				outArgs.status = 'vertical';
				return
			end
			outArgs.result(3) = -az / (V * cgamma);
		end
		% 计算速度大小、速度方向角变化率关于弹道系加速度矢量的偏导数矩阵
		function outArgs = calc_par_dvgp_par_a(vgp, g)
			arguments
				vgp	(3, 1)	double	% [V; gamma; psi]
				g	(1, 1)	double	% 重力加速度
			end
			outArgs = struct();
			outArgs.status = 'ok';
			% 原理详见文档
			V = vgp(1);
			gamma = vgp(2);
			outArgs.result = diag([1.0, zeros(1, 2)]);
			if V == 0.0
				outArgs.status = 'zeroR';
				return
			end
			outArgs.result(2, 2) = 1.0 / V;
			cgamma = cos(gamma);
			if V * cgamma == 0.0
				outArgs.status = 'vertical';
				return
			end
			outArgs.result(3, 3) = -1.0 / (V * cgamma);
		end
		% 计算速度大小、速度方向角变化率关于速度大小、速度方向角的偏导数矩阵
		function outArgs = calc_par_dvgp_par_vgp(inArgs)
			arguments
				inArgs.a	(3, 1)	double	% 弹道系加速度矢量 [ax; ay; az]
				inArgs.vgp	(3, 1)	double	% [V; gamma; psi]
				inArgs.g	(1, 1)	double	% 重力加速度
			end
			outArgs = struct();
			outArgs.status = 'ok';
			% 原理详见文档
			ay = inArgs.a(2);
			az = inArgs.a(3);
			V = inArgs.vgp(1);
			gamma = inArgs.vgp(2);
			cgamma = cos(gamma);
			g = inArgs.g;
			outArgs.result = [
				0.0,	-g * cgamma,	0.0;
				zeros(2, 3)
			];
			if V^2 == 0.0
				outArgs.status = 'zeroR';
				return
			end
			sgamma = sin(gamma);
			outArgs.result(2, 1: 2) = [-(ay - g * cgamma) / V^2, g * sgamma / V];
			if (V * cgamma)^2 == 0.0
				outArgs.status = 'vertical';
				return
			end
			outArgs.result(3, 1: 2) = [az / (V^2 * cgamma), -az * sgamma / (V * cgamma^2)];
		end
	end
end