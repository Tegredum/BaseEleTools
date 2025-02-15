% encoding: utf-8
% author: Tegredum
% MATLAB version: R2024b
% 提供若干常用变换算法接口（三维速度矢量与速度大小方向间的变换、坐标系变换等）
classdef TransUtils_teg
	methods(Static)
		% 将速度大小、方向转换为速度矢量，遵循北天东定义
		function velVec = velDir_2_velVec(inArgs)
			arguments(Input)
				inArgs.V		(1, 1)	double	% 速度大小
				inArgs.theta	(1, 1)	double	% 弹道倾角（rad）
				inArgs.psi		(1, 1)	double	% 弹道偏角（rad）
			end
			arguments(Output)
				velVec	(3, 1)	double	% 速度矢量
			end
			theta = inArgs.theta;
			psi = inArgs.psi;
			ctheta = cos(theta);
			stheta = sin(theta);
			cpsi = cos(psi);
			spsi = sin(psi);
			velVec = [
				ctheta * cpsi;
				stheta;
				-ctheta * spsi
			] * inArgs.V;
		end
		% 将速度矢量转换为速度大小、方向，遵循北天东定义
		function outArgs = velVec_2_velDir(inVec)
			arguments
				inVec	(3, 1)	double	% 输入速度矢量
			end
			V = norm(inVec);
			outArgs = struct('V', V);
			if V == 0
				outArgs.status = 'zeroVec';
				return
			end
			vy = inVec(2);
			theta = asin(vy / V);
			outArgs.theta = theta;
			vx = inVec(1);
			vz = inVec(3);
			if vx == 0 && vz == 0
				outArgs.status = 'verticalVec';
				return
			end
			psi = atan2(-vz, vx);
			outArgs.psi = psi;
			outArgs.status = 'ok';
		end
		% 按照从速度大小与方向转换至速度矢量的公式，计算速度矢量关于速度大小的偏导数。原理公式见文档
		function result = par_velVec_par_V(inArgs)
			arguments(Input)
				inArgs.theta	(1, 1)	double	% 弹道倾角（rad）
				inArgs.psi		(1, 1)	double	% 弹道偏角（rad）
			end
			arguments(Output)
				result	(3, 1)	double	% 速度矢量关于速度大小的偏导数
			end
			result = TransUtils_teg.velDir_2_velVec(...
				'V', 1.0,...
				'theta', inArgs.theta,...
				'psi', inArgs.psi);
		end
		% 按照从速度大小与方向转换至速度矢量的公式，计算速度矢量关于弹道倾角的偏导数。原理公式见文档
		function result = par_velVec_par_theta(inArgs)
			arguments(Input)
				inArgs.V		(1, 1)	double	% 速度大小
				inArgs.theta	(1, 1)	double	% 弹道倾角（rad）
				inArgs.psi		(1, 1)	double	% 弹道偏角（rad）
			end
			arguments(Output)
				result	(3, 1)	double	% 速度矢量关于弹道倾角的偏导数
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
			] * inArgs.V;
		end
		% 按照从速度大小与方向转换至速度矢量的公式，计算速度矢量关于弹道偏角的偏导数。原理公式见文档
		function result = par_velVec_par_psi(inArgs)
			arguments(Input)
				inArgs.V		(1, 1)	double	% 速度大小
				inArgs.theta	(1, 1)	double	% 弹道倾角（rad）
				inArgs.psi		(1, 1)	double	% 弹道偏角（rad）
			end
			arguments(Output)
				result	(3, 1)	double	% 速度矢量关于弹道偏角的偏导数
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
			] * (-inArgs.V * ctheta);
		end
	end
end