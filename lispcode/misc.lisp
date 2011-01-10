;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;; This file is licensed under the terms of MIT license,
;;;; see LICENSE file.
;;;;
;;;; Some esoteric stuff.

;;;
;;; This is obvious.
;;;
(label lcons
       (lambda (p q)
         (lambda (m) (m p q))))

(label lcar
       (lambda (z)
         (z (lambda (p q) p))))

(label lcdr
       (lambda (z)
         (z (lambda (p q) q))))
;;;
;;; The Y combinator!
;;;
(label y-comb
       (lambda (m)
         ((lambda (f) (m (lambda (a) ((f f) a))))
          (lambda (f) (m (lambda (a) ((f f) a)))))))
;; Example: factorial based on Y-comb
;; ((y-comb (lambda (r)
;;            (lambda (n)
;;              (if (= n 0)
;;                  1
;;                  (* n (r (- n 1))))))) 5) => 120

;; Example: length of the list
;; ((y-comb (lambda (r)
;;            (lambda (l)
;;              (if (null l)
;;                  0
;;                  (+ 1 (r (cdr l))))))) '(a b c d e)) => 5

;;;
;;; Continuation passing style.
;;;

;; Factorial
(label fact-cps
       (lambda (n k)
         (if (= n 0)
             (k 1)
             (fact-cps (- n 1)
                       (lambda (v)
                         (k (* n v)))))))
;; Example: (fact-cps 5 (lambda (v) v)) => 120

;; Length
(label length-cps
       (lambda (lyst k)
         (cond ((null lyst) (k 0))
               (t (length-cps (cdr lyst)
                              (lambda (v)
                                (k (+ 1 v))))))))
;; Example: (length-cps '(a b c d e f) (lambda (v) v)) => 6

;; Map
(label map-cps
       (lambda (proc args k)
         (if (null args)
             (k nil)
             (map-cps proc (cdr args) (lambda (v)
                                        (k (cons (proc (car args)) v)))))))
;; Example: (map-cps (lambda (x) (* x x))
;;                   '(1 2 3 4 5) (lambda (v) v)) => '(1 4 9 16 25)
