% encoding: utf-8
% author: Tegredum
% MATLAB version: R2024b
% 提供若干常用变换算法接口（三维速度矢量与速度大小方向间的变换、坐标系变换等）
classdef TransUtils_teg
	methods(Static)
		% 将矢量大小、方向转换为矢量本身，遵循北天东定义
		function vec = normDir_2_vec(inArgs)
			arguments(Input)
				inArgs.norm		(1, 1)	double	% 矢量大小
				inArgs.theta	(1, 1)	double	% 矢量倾角（rad）
				inArgs.psi		(1, 1)	double	% 矢量偏角（rad）
			end
			arguments(Output)
				vec		(3, 1)	double	% 矢量本身
			end
			theta = inArgs.theta;
			psi = inArgs.psi;
			ctheta = cos(theta);
			stheta = sin(theta);
			cpsi = cos(psi);
			spsi = sin(psi);
			vec = [
				ctheta * cpsi;
				stheta;
				-ctheta * spsi
			] * inArgs.norm;
		end
		% 将矢量本身转换为矢量大小、方向，遵循北天东定义
		function outArgs = vec_2_normDir(inVec)
			arguments
				inVec	(3, 1)	double	% 输入矢量本身
			end
			r = norm(inVec);
			outArgs = struct('norm', r);
			if r == 0
				outArgs.status = 'zeroVec';
				return
			end
			ry = inVec(2);
			theta = asin(ry / r);
			outArgs.theta = theta;
			rx = inVec(1);
			rz = inVec(3);
			if rx == 0 && rz == 0
				outArgs.status = 'verticalVec';
				return
			end
			psi = atan2(-rz, rx);
			outArgs.psi = psi;
			outArgs.status = 'ok';
		end
		% 按照从矢量大小与方向转换至矢量本身的公式，计算矢量关于矢量大小的偏导数。原理公式见文档
		function result = par_vec_par_norm(inArgs)
			arguments(Input)
				inArgs.theta	(1, 1)	double	% 矢量倾角（rad）
				inArgs.psi		(1, 1)	double	% 矢量偏角（rad）
			end
			arguments(Output)
				result	(3, 1)	double	% 矢量本身关于矢量大小的偏导数
			end
			result = TransUtils_teg.normDir_2_vec(...
				'norm', 1.0,...
				'theta', inArgs.theta,...
				'psi', inArgs.psi);
		end
		% 按照从矢量大小与方向转换至矢量本身的公式，计算矢量关于矢量倾角的偏导数。原理公式见文档
		function result = par_vec_par_theta(inArgs)
			arguments(Input)
				inArgs.norm		(1, 1)	double	% 矢量大小
				inArgs.theta	(1, 1)	double	% 矢量倾角（rad）
				inArgs.psi		(1, 1)	double	% 矢量偏角（rad）
			end
			arguments(Output)
				result	(3, 1)	double	% 矢量本身关于矢量倾角的偏导数
			end
			theta = inArgs.theta;
			psi = inArgs.psi;
			ctheta = cos(theta);
			stheta = sin(theta);
			cpsi = cos(psi);
			spsi = sin(psi);
			result = [
				-stheta * cpsi;
				ctheta;
				stheta * spsi
			] * inArgs.norm;
		end
		% 按照从矢量大小与方向转换至矢量本身的公式，计算矢量关于矢量偏角的偏导数。原理公式见文档
		function result = par_vec_par_psi(inArgs)
			arguments(Input)
				inArgs.norm		(1, 1)	double	% 矢量大小
				inArgs.theta	(1, 1)	double	% 矢量倾角（rad）
				inArgs.psi		(1, 1)	double	% 矢量偏角（rad）
			end
			arguments(Output)
				result	(3, 1)	double	% 矢量本身关于矢量偏角的偏导数
			end
			theta = inArgs.theta;
			psi = inArgs.psi;
			ctheta = cos(theta);
			cpsi = cos(psi);
			spsi = sin(psi);
			result = [
				spsi;
				0.0;
				cpsi
			] * (-inArgs.norm * ctheta);
		end
		% 将上述三个方法合并，计算重构矢量关于输入矢量大小、方向角度偏导矩阵
		function result = par_vec_par_rtp(rtp)
			arguments
				rtp	(3, 1)	double	% [r; theta; psi]
			end
			r = rtp(1);
			theta = rtp(2);
			psi = rtp(3);
			result = [
				TransUtils_teg.par_vec_par_norm(...
					'norm', r,...
					'theta', theta,...
					'psi', psi),...
				TransUtils_teg.par_vec_par_theta(...
					'norm', r,...
					'theta', theta,...
					'psi', psi),...
				TransUtils_teg.par_vec_par_psi(...
					'norm', r,...
					'theta', theta,...
					'psi', psi)
			];
		end
		% 计算重构矢量关于输入矢量大小、方向角度偏导矩阵的逆矩阵，公式详见文档
		function outArgs = par_vec_par_rtp_inv(rtp)
			arguments
				rtp	(3, 1)	double	% [r; theta; psi]
			end
			outArgs = struct();
			outArgs.status = 'ok';
			r = rtp(1);
			theta = rtp(2);
			psi = rtp(3);
			ctheta = cos(theta);
			stheta = sin(theta);
			cpsi = cos(psi);
			spsi = sin(psi);
			% 结果矩阵
			outArgs.result = [
				ctheta * cpsi,	stheta,	-ctheta * spsi;
				zeros(2, 3)
			];
			if r == 0.0
				% r 为 0 时，将结果矩阵后两行置零
				outArgs.status = 'zeroR';
				return
			end
			outArgs.result(2, :) = [-stheta * cpsi, ctheta, stheta * spsi] / r;
			if r * ctheta == 0.0
				% cos(theta) 为 0 时，将结果矩阵最后一行置零
				outArgs.status = 'vertical';
				return
			end
			outArgs.result(3, :) = [spsi, 0.0, cpsi] / (-r * ctheta);
		end
		% 计算某个逆矩阵关于某个输入的偏导数的，详见文档
		function outArgs = par_par_vec_par_rtp_inv_r(rtp)
			arguments
				rtp	(3, 1)	double	% [r; theta; psi]
			end
			outArgs = struct();
			outArgs.status = 'ok';
			r = rtp(1);
			theta = rtp(2);
			psi = rtp(3);
			ctheta = cos(theta);
			stheta = sin(theta);
			cpsi = cos(psi);
			spsi = sin(psi);
			% 结果矩阵
			outArgs.result = zeros(3, 3);
			if r^2 == 0.0
				% r 为 0 时，将结果矩阵后两行置零
				outArgs.status = 'zeroR';
				return
			end
			outArgs.result(2, :) = [-stheta * cpsi, ctheta, stheta * spsi] / (-r^2);
			if r^2 * ctheta == 0.0
				% cos(theta) 为 0 时，将结果矩阵最后一行置零
				outArgs.status = 'vertical';
				return
			end
			outArgs.result(3, :) = [spsi, 0.0, cpsi] / (r^2 * ctheta);
		end
		% 计算某个逆矩阵关于某个输入的偏导数的，详见文档
		function outArgs = par_par_vec_par_rtp_inv_t(rtp)
			arguments
				rtp	(3, 1)	double	% [r; theta; psi]
			end
			outArgs = struct();
			outArgs.status = 'ok';
			r = rtp(1);
			theta = rtp(2);
			psi = rtp(3);
			ctheta = cos(theta);
			stheta = sin(theta);
			cpsi = cos(psi);
			spsi = sin(psi);
			% 结果矩阵
			outArgs.result = [
				-stheta * cpsi,	ctheta,	stheta * spsi;
				zeros(2, 3)
			];
			if r == 0.0
				% r 为 0 时，将结果矩阵后两行置零
				outArgs.status = 'zeroR';
				return
			end
			outArgs.result(2, :) = [-ctheta * cpsi, -stheta, ctheta * spsi] / r;
			if r * ctheta^2 == 0.0
				% cos(theta) 为 0 时，将结果矩阵最后一行置零
				outArgs.status = 'vertical';
				return
			end
			outArgs.result(3, :) = [spsi, 0.0, cpsi] * (-stheta / (r * ctheta^2));
		end
		% 计算某个逆矩阵关于某个输入的偏导数的，详见文档
		function outArgs = par_par_vec_par_rtp_inv_p(rtp)
			arguments
				rtp	(3, 1)	double	% [r; theta; psi]
			end
			outArgs = struct();
			outArgs.status = 'ok';
			r = rtp(1);
			theta = rtp(2);
			psi = rtp(3);
			ctheta = cos(theta);
			stheta = sin(theta);
			cpsi = cos(psi);
			spsi = sin(psi);
			% 结果矩阵
			outArgs.result = [
				[spsi,	0.0,	cpsi] * (-ctheta);
				zeros(2, 3)
			];
			if r == 0.0
				% r 为 0 时，将结果矩阵后两行置零
				outArgs.status = 'zeroR';
				return
			end
			outArgs.result(2, :) = [spsi, 0.0, cpsi] * (stheta / r);
			if r * ctheta == 0.0
				% cos(theta) 为 0 时，将结果矩阵最后一行置零
				outArgs.status = 'vertical';
				return
			end
			outArgs.result(3, :) = [-cpsi, 0.0, spsi] / (r * ctheta);
		end
		% 通过 index 调用上面的逆矩阵偏导计算方法
		function outArgs = par_par_vec_par_rtp_inv_byIdx(rtp, idx)
			arguments
				rtp	(3, 1)	double	% [r; theta; psi]
				idx	(1, 1)	double	% 偏导索引
			end
			switch idx
				case 0
					outArgs = TransUtils_teg.par_par_vec_par_rtp_inv_r(rtp);
				case 1
					outArgs = TransUtils_teg.par_par_vec_par_rtp_inv_t(rtp);
				case 2
					outArgs = TransUtils_teg.par_par_vec_par_rtp_inv_p(rtp);
				otherwise
					error('Invalid index')
			end
		end
		% 旋转矩阵 RY
		function R = rotMatY(theta)
			arguments(Input)
				theta	(1, 1)	double	% 旋转角度（rad）
			end
			arguments(Output)
				R		(3, 3)	double	% 旋转矩阵
			end
			ctheta = cos(theta);
			stheta = sin(theta);
			R = [
				ctheta,		0.0,	stheta;
				0.0,		1.0,	0.0;
				-stheta,	0.0,	ctheta
			];
		end
		% 旋转矩阵 RZ
		function R = rotMatZ(theta)
			arguments(Input)
				theta	(1, 1)	double	% 旋转角度（rad）
			end
			arguments(Output)
				R		(3, 3)	double	% 旋转矩阵
			end
			ctheta = cos(theta);
			stheta = sin(theta);
			R = [
				ctheta,		-stheta,	0.0;
				stheta,		ctheta,		0.0;
				0.0,		0.0,		1.0
			];
		end
		% 将输入向量在地面系下的坐标转换为弹道系下的坐标
		function outVec = earth2orbit(inVec, theta, psi)
			arguments(Input)
				inVec	(3, 1)	double	% 输入矢量本身
				theta	(1, 1)	double	% 弹道倾角（rad）
				psi		(1, 1)	double	% 弹道偏角（rad）
			end
			arguments(Output)
				outVec	(3, 1)	double	% 输出矢量本身
			end
			% 原理详见文档
			RZ = TransUtils_teg.rotMatZ(-theta);
			RY = TransUtils_teg.rotMatY(-psi);
			outVec = RZ * (RY * inVec);
		end
		% 将输入向量在弹道系下的坐标转换为地面系下的坐标
		function outVec = orbit2earth(inVec, theta, psi)
			arguments(Input)
				inVec	(3, 1)	double	% 输入矢量本身
				theta	(1, 1)	double	% 弹道倾角（rad）
				psi		(1, 1)	double	% 弹道偏角（rad）
			end
			arguments(Output)
				outVec	(3, 1)	double	% 输出矢量本身
			end
			% 原理详见文档
			RY = TransUtils_teg.rotMatY(psi);
			RZ = TransUtils_teg.rotMatZ(theta);
			outVec = RY * (RZ * inVec);
		end
	end
end