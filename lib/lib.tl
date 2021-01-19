(defun not (x)
	(if (eq x 0) 1 0))

(defun and (x y)
	(if x
		(if y 1 0) 0))

(defun or (x y)
	(if x
		1 (if y 1 0)))

(defun null (x) (eq x nil))

(defun atom (x)
	(if (and (not (consp x))
			(not (null x)))
		1 0))

(defun map (foo lst)
	(if (eq lst nil)
		nil
		(cons
			(foo (car lst))
			(map foo (cdr lst)))))

(defun fib (x)
	(if (eq x 1)
		1
		(if (eq x 2)
			1
			(+ (fib (- x 2)) (fib (- x 1) )))))