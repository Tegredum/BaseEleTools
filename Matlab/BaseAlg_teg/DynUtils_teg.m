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
	end
end