classdef Poly_teg < handle
	% Polynomial class implementation
	
	properties (SetAccess = private)
		coeffs       % Polynomial coefficients array
		maxDegree    % Maximum allowed degree
		actualDegree % Actual highest degree
	end
	
	methods
		function obj = Poly_teg(maxDegree, initialCoeffs)
			% Constructor
			% Inputs: 
			%   maxDegree - Maximum allowed degree
			%   initialCoeffs - Initial coefficients array
			
			% Initialize coefficients array size
			obj.maxDegree = maxDegree;
			obj.coeffs = zeros(1, maxDegree + 1);
			
			% Set initial coefficients
			if nargin > 1
				obj.setCoefficients(initialCoeffs);
			end
		end
		
		function clearCoefficients(obj)
			% Clear all coefficients to zero
			obj.coeffs = zeros(1, obj.maxDegree + 1);
			obj.actualDegree = 0;
		end
		
		function setCoefficients(obj, newCoeffs)
			% Set polynomial coefficients
			% Input: newCoeffs - New coefficients array
			
			% Check input length
			if length(newCoeffs) > (obj.maxDegree + 1)
				error('Input coefficients array exceeds maximum allowed degree');
			end
			
			% Update coefficients
			obj.coeffs(1:length(newCoeffs)) = newCoeffs;
			
			% Set actual highest degree
			obj.actualDegree = numel(newCoeffs) - 1;
		end
		
		function c = getCoefficient(obj, degree)
			% Get coefficient at specified degree
			% Input: degree - Degree
			% Output: Corresponding coefficient
			
			% arguments(Input)
			%   degree (1,1) double {mustBeInteger, mustBeNonnegative}
			% arguments(Output)
			%   c (1,1) double
			% end
			
			if degree > obj.actualDegree
				error('Requested degree exceeds maximum allowed degree');
			end
			c = obj.coeffs(degree + 1);
		end
		
		function setCoefficient(obj, degree, value)
			% Set coefficient at specified degree
			% Inputs: 
			%   degree - Degree
			%   value - Coefficient value
			
			% arguments(Input)
			%   degree (1,1) double {mustBeInteger, mustBeNonnegative}
			%   value (1,1) double
			% end
			
			if degree > obj.maxDegree
				error('Requested degree exceeds maximum allowed degree');
			end
			
			% Update coefficient
			obj.coeffs(degree + 1) = value;
			
			% Update actual degree if needed
			if degree > obj.actualDegree
				obj.actualDegree = degree;
			end
		end
		
		function p = plus(obj, other)
			% Polynomial addition
			% Input: other - Another polynomial object or scalar
			% Output: New polynomial object
			
			% arguments(Input)
			%   other (1,1) {mustBeA(other, ['Poly_teg', 'double'])}
			% arguments(Output)
			%   p (1,1) Poly_teg
			% end
			
			if isnumeric(other)
				% Polynomial plus scalar
				p = Poly_teg(obj.maxDegree);
				% Copy original coefficients
				for i = 0:obj.actualDegree
					p.setCoefficient(i, obj.getCoefficient(i));
				end
				% Add scalar value
				p.setCoefficient(0, p.getCoefficient(0) + other);
			else
				% Two polynomials addition
				maxDeg = max(obj.maxDegree, other.maxDegree);
				p = Poly_teg(maxDeg);
				
				% Copy first polynomial coefficients
				for i = 0:obj.actualDegree
					p.setCoefficient(i, obj.getCoefficient(i));
				end
				
				% Add second polynomial coefficients
				for i = 0:other.actualDegree
					p.setCoefficient(i, p.getCoefficient(i) + other.getCoefficient(i));
				end
			end
		end
		
		function p = mtimes(obj, other)
			% Polynomial multiplication or scalar multiplication
			% Input: other - Another polynomial object or scalar
			% Output: New polynomial object
			
			% arguments(Input)
			%   other (1,1) {mustBeA(other, ['Poly_teg', 'double'])}
			% arguments(Output)
			%   p (1,1) Poly_teg
			% end
			
			if isnumeric(other)
				% Scalar multiplication
				p = Poly_teg(obj.maxDegree);
				for i = 0:obj.actualDegree
					p.setCoefficient(i, obj.getCoefficient(i) * other);
				end
			else
				% Polynomial multiplication
				maxDeg = obj.maxDegree + other.maxDegree;
				p = Poly_teg(maxDeg);
				
				% Implement polynomial multiplication
				for i = 0:obj.actualDegree
					for j = 0:other.actualDegree
						deg = i + j;
						if deg <= p.maxDegree
							current = p.getCoefficient(deg);
							p.setCoefficient(deg, current + ...
								obj.getCoefficient(i) * other.getCoefficient(j));
						end
					end
				end
			end
		end

		function dp = derivative(obj)
			% Polynomial derivative
			% Output: New polynomial object representing derivative
			
			% arguments(Output)
			%   dp (1,1) Poly_teg
			% end
			
			% Create derivative polynomial, maintain original maxDegree
			dp = Poly_teg(max(obj.maxDegree - 1, 0));
			
			if obj.actualDegree > 0
				% Calculate derivative coefficients
				for i = 1:obj.actualDegree
					dp.setCoefficient(i-1, obj.getCoefficient(i) * i);
				end
			end
		end
		
		function y = evaluate(obj, x)
			% Polynomial evaluation
			% Input: x - Variable value
			% Output: Polynomial value at x
			
			% arguments(Input)
			%   x (1,1) double
			% arguments(Output)
			%   y (1,1) double
			% end
			
			y = 0;
			for i = 0:obj.actualDegree
				y = y + obj.getCoefficient(i) * x^i;
			end
		end
	end
end