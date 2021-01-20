(print 'Is '10 'even)

(letrec
	(
		(isOdd (lambda (x) (if (eq x 0)
			0
			(isEven (- x 1)))))
		(isEven (lambda (x) (if (eq x 0)
			1
			(isOdd (- x 1)))))
	)
	(isEven 10)
)

(print '10 'th 'Fibonacci 'number)

(letrec
	(
		(fib (lambda (x) (if (eq x 1)
			1
			(if (eq x 2)
				1
				(+ (fib (- x 1)) (fib (- x 2))))
			)))
	)
	(fib 10)
)