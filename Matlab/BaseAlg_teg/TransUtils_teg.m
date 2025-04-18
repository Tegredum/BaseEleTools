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
	end
end