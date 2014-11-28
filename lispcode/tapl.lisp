;;
;; Playground for untyped LC during reading TaPL book.
;;
;; Note:
;;   Since we don't have carrying in the interpeter we must define
;;   functions which can apply on many arguments as function with only
;;   one argument which returns another function from one argument, and so on.
;;   Furthermore, more verbose declarations of λ-combinators are easier to understand
;;   and read, because it's more like a regular notation for λ calculus.
;;
;; Head 4.2
;; Programming in the Lambda-Calculus

;;----------------------------------------------------------------------------;;
;; Head 4.2.1
;;   Lambda combinators given by the book
;;----------------------------------------------------------------------------;;

(label tru (lambda (t)
             (lambda (f)
               t)))

(label fls (lambda (t)
             (lambda (f)
               f)))

(label test (lambda (l)
              (lambda (m)
                (lambda (n)
                  (l m n)))))

;; Logical AND cases:
;;   ((and tru fls) t f) => f
;;   ((and tru tru) t f) => t
;;   ((and fls tru) t f) => f
;;   ((and fls fls) t f) => f
(label mand (lambda (b)
              (lambda (c)
                ((b c) fls))))

;;----------------------------------------------------------------------------;;
;; Exercises 4.2.1:
;;   Define logical 'or', and 'not' combinators
;;----------------------------------------------------------------------------;;

;; Logical OR cases:
;;   ((or tru fls) t f) => t
;;   ((or tru tru) t f) => t 
;;   ((or fls tru) t f) => t
;;   ((or fls fls) t f) => f
(label mor (lambda (b)
             (lambda (c)
               ((b tru) c))))

;; Logical NOT cases:
;;   ((not tru) t f) => f
;;   ((not fls) t f) => t
(label mnot (lambda (p)
              ((p fls) tru)))

;;----------------------------------------------------------------------------;;
;; Head 4.2.2
;;   Pair of values
;;   Church Numerals
;;----------------------------------------------------------------------------;;

(label mpair (lambda (f)
               (lambda (s)
                 (lambda (b)
                   ((b f) s)))))

;; (fst ((mpair 1) 2)) => 1
(label fst (lambda (p)
             (p tru)))

;; (snd ((mpair 1) 2)) => 2
(label snd (lambda (p)
             (p fls)))

;;----------------------------------------------------------------------------;;
;; Just a several 'constants'
;;

(label c0 (lambda (s)
            (lambda (z)
              z)))

(label c1 (lambda (s)
            (lambda (z)
              (s z)))) ;; Or you can substitute it with (s ((c0 s) z))

(label c2 (lambda (s)
            (lambda (z)
              (s (s z))))) ;; same here: (s ((c1 s) z))

(label c3 (lambda (s)
            (lambda (z)
              (s ((c2 s) z)))))

;; Generally you can do (s ((cN s) z)), so you need only c0 defined which
;; implies zero.

;;----------------------------------------------------------------------------;;
;; Exercise 4.2.3
;;   Define term for calculating the successor of a number
;;----------------------------------------------------------------------------;;

;; Here we actually add natural numbers to our language, without it
;; we can't think about Church numerals as a numbers, without it it's just
;; an abstraction.

;; At first I thought that zero should return function, but then I realized
;; that here is the place to be breaking of abstraction.
;;
;; (label zero (lambda (r) 0)) 
;;
;; So, zero and succ should operate on natural numbers and nothing else.
;; It's obvious that with such semantics we can't do something like that (succ c1) => c2

(label zero 0)
(label succ (lambda (r)
              (progn
                ;; (display "Ding")
                ;; (newline)
                (+ 1 r))))

;; Solutions from the book
;;
;; (label succ1 (lambda (n)
;;                (lambda (s)
;;                  (lambda (z)
;;                    (s ((n s) z))))))
;;
;; (label succ2 (lambda (n)
;;                (lambda (s)
;;                  (lambda (z)
;;                    ((n s) (s z))))))
;;
;; ;; Actually, this was the first solution that I've figured out
;; (label succ3 (lambda (n)
;;                ((plus c1) n)))
;;


;;----------------------------------------------------------------------------;;
;; Mind blowning staff starts here
;;

(label plus (lambda (m)
              (lambda (n)
                (lambda (s)
                  (lambda (z)
                    ((m s) ((n s) z)))))))

;; Synonym for multiplication
(label times (lambda (m)
               (lambda (n)
                 ((m (plus n)) c0))))

;;----------------------------------------------------------------------------;;
;; Exercise 4.2.4
;;   Define power combinator
;;----------------------------------------------------------------------------;;
(label pow (lambda (m)
             (lambda (n)
               ((n (times m)) c1))))

;;----------------------------------------------------------------------------;;
;; The madness continues...
;;

;; (((iszro c1) t) nil) => nil
;; (((iszro c0) t) nil) => t
(label iszro (lambda (m)
               ((m (lambda (x) fls)) tru)))

;; What the hack is going on here? Let's look closer.
;;
;; Here we again have a broken abstraction. We need prd that can
;; work directly with natural numbers as well as it does the succ combinator,
;; but book gives us these declarations.
(label zz ((mpair c0) c0))
(label ss (lambda (p)
            ((mpair (snd p)) ((plus c1) (snd p)))))
(label prd (lambda (m)
             (fst ((m ss) zz))))
