;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;; This file is licensed under the terms of MIT license,
;;;; see LICENSE file.
;;;;
;;;; Math things.

;; Factorial
;; Recursive function
(label factorial
       (lambda (n)
         (if (= n 0)
             1
             (* n (factorial (- n 1))))))

;; Factorial-iter
;; Iterative function with implicit
;; definition and calling inner function
(label factorial-iter
       (lambda (n)
         (label fact-iter
                (lambda (n count acc)
                  (if (> count n)
                      acc
                      (fact-iter n (+ count 1) (* count acc)))))
         (fact-iter n 1 1)))

;; Fibonacci
;; Recurcive function
;; Return list of the first N fibonacci numbers
(label fibonacci
       (lambda (n)
         (label fibo
                (lambda (n a b)
                  (if (= n 0)
                      nil
                      (cons a (fibo (- n 1) b (+ a b))))))
         (fibo n 0 1)))

;; Square
(label square
       (lambda (x) (* x x)))

;; Cube
(label cube
       (lambda (x) (* x x x)))

(label gcd
       (lambda (a b)
         (if (= b 0)
             a
             (gcd b (mod a b)))))

