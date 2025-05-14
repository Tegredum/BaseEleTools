% encoding: utf-8
% author: Tegredum
% matlab version: R2024b
classdef LinAlg_teg
	methods(Static)
		function [H, v, y] = householderTransform(inVec)
			% 输入验证
			if ~isvector(inVec)
				error('Input must be a vector');
			end
			
			% 获取向量长度
			n = length(inVec);
			
			% 构造单位向量e1
			e1 = zeros(n, 1);
			e1(1) = 1;
			
			% 计算v向量
			v = e1 * norm(inVec);
			
			% 计算y向量并归一化
			y = LinAlg_teg.normalize(inVec(:) - v);
			
			% 计算Householder变换矩阵H
			H = eye(n) - 2 * (y * y');
		end
		
		function normalizedVec = normalize(inVec)
			% 输入验证
			if ~isvector(inVec)
				error('Input must be a vector');
			end
			
			% 计算向量范数
			n = norm(inVec);
			
			% 处理零向量情况
			if n == 0
				normalizedVec = zeros(size(inVec(:)));
			else
				% 归一化
				normalizedVec = inVec(:) / n;
			end
		end
	end
end