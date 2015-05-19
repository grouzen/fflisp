;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;; This file is licensed under the terms of MIT license,
;;;; see LICENSE file.
;;;;
;;;; Some useful function which must be
;;;; in every lisp implementation.

;; Map
(label map
       (lambda (proc args)
         (if (null args)
             nil
             (cons (proc (car args)) (map proc (cdr args))))))

;; Reduce
(label reduce
       (lambda (proc args)
         (if (null (cdr args))
             (car args)
             (reduce proc
                     (cons (proc (car args) (car (cdr args)))
                           (cdr (cdr args)))))))

;; Foldl
;; (proc (proc (proc (proc seq1 start) seq2) seq3) seq4)
;; ((((seq1 start) seq2) seq3) seq4)
(label fold-left
       (lambda (proc start seq)
         (if (null seq)
             start
             (fold-left proc
                        (proc start (car seq))
                        (cdr seq)))))

;; Foldr
;; (proc seq1 (proc seq2 (proc seq3 (proc seq4 start))))
;; (seq1 (seq2 (seq3 (seq4 start))))
(label fold-right
       (lambda (proc start seq)
         (if (null seq)
             start
             (proc (car seq)
                   (fold-right proc start (cdr seq))))))

;; Find
(label find
       (lambda (item seq)
         (cond ((null seq) nil)
               ((equal item (car seq)) (car seq))
               (t (find item (cdr seq))))))

;; Length
(label length
       (lambda (list)
         (if (null list)
             0
             (+ 1 (length (cdr list))))))

;; Append
(label append
       (lambda (list elem)
         (if (null list)
             elem
             (cons (car list) (append (cdr list) elem)))))

;; Remove-if
(label remove-if
       (lambda (pred seq)
         (cond ((null seq) nil)
               ((pred (car seq)) (remove-if pred (cdr seq)))
               (t (cons (car seq) (remove-if pred (cdr seq)))))))

;; Find-if
(label find-if
       (lambda (pred seq)
         (cond ((null seq) nil)
               ((pred (car seq)) (cons (car seq) (find-if pred (cdr seq))))
               (t (find-if pred (cdr seq))))))

;; Reverse
(label reverse
       (lambda (lyst)
         (if (null lyst)
             nil
             (append (reverse (cdr lyst))
                     (list (car lyst))))))

;; Pair-copy
(label pair-copy
       (lambda (pr)
         (cons (car pr) (cdr pr))))

;;;;;;;;;;;;;;;;;;;;;;;;;
;;; (assp proc alist)
;;; (assoc obj alist)
;;; (assq obj alist)

(label assp
       (lambda (proc alist)
         (cond ((null alist) nil)
               ((proc (car (car alist))) (car alist))
               (t (assp proc (cdr alist))))))

(label assoc
       (lambda (obj alist)
         (cond ((null alist) nil)
               ((equal (car (car alist)) obj) (car alist))
               (t (assoc obj (cdr alist))))))

(label assq
       (lambda (obj alist)
         (cond ((null alist) nil)
               ((eq (car (car alist)) obj) (car alist))
               (t (assq obj (cdr alist))))))
