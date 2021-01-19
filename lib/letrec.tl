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
