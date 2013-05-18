;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;; This file is licensed under the terms of MIT license,
;;;; see LICENSE file.
;;;; 
;;;; Two lists intersection function.
;;;;

(label cartesian
       (lambda (a b)
         (reduce append
                 (map (lambda (x)
                        (map (lambda (y)
                               (cons x y)) b))
                      a))))

(label intersect
       (lambda (a b)
         (map (lambda (x)
                (car x))
              (remove-if (lambda (x)
                           (not (= (car x) (cdr x))))
                         (cartesian a b)))))
       
