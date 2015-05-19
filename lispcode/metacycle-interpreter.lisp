;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;; This file is licensed under the terms of MIT license,
;;;; see LICENSE file.
;;;;
;;;; Metacycle interpreter.

;; Metacycle eval.
(label meval
       (lambda (exp env)
         (cond ((self-evaluating-p exp) exp)
               ((variable-p exp) (lookup-variable-value exp env))
               ((quoted-p exp) (car (cdr exp)))
               ((let-p exp) (meval (let-to-combination exp)))
               ((assignment-p exp) (eval-assignment exp env))
               ((definition-p exp) (eval-definition exp env))
               ((if-p exp) (eval-if exp env))
               ((lambda-p exp)
                (make-procedure (lambda-parameters exp)
                                (lambda-body exp)
                                env))
               ((progn-p exp)
                (eval-sequence (cdr exp) env))
               ((cond-p exp) (meval (cond-to-if exp) env))
               ((application-p exp)
                (mapply (meval (car exp) env)
                        (list-of-values (cdr exp) env)))
               (t (error "Unknown meval form")))))

;; Metacycle apply
(label mapply
       (lambda (procedure arguments)
         (cond ((primitive-procedure-p procedure)
                (apply-primitive-procedure procedure arguments))
               ((compound-procedure-p procedure)
                (eval-sequence (car (cdr (cdr procedure)))
                               (extend-environment
                                (car (cdr procedure))
                                arguments
                                (car (cdr (cdr (cdr procedure)))))))
               (t (error "Unknown mapply procedure")))))

(label tagged-list-p
       (lambda (exp tag)
         (if (pair exp)
             (eq (car exp) tag)
             nil)))

(label extend-environment
       (lambda (vars vals env)
         (if (= (length vars) (length vals))
             (cons (cons vars vals) env)
             (error "Wrong number of arguments"))))

(label mrepl
       (lambda ()
         (display "meta-fflisp> ")
         (let ((input (read)))
           (if (eq input 'exit)
               t
               (let ((output (meval input environment)))
                 (display "meta=> ")
                 (if (compound-procedure-p output)
                     (display (list 'compound-procedure
                                    (car (cdr output))
                                    'proc-env))
                     (display output))
                 (newline)
                 (mrepl))))))
       
;; User defined case.
(label compound-procedure-p
       (lambda (proc)
         (tagged-list-p proc 'procedure)))
              
;; Primitive case.
(label primitive-procedure-p
       (lambda (proc)
         (tagged-list-p proc 'primitive)))

(label primitive-procedures
       (list (list 'car car)
             (list 'cdr cdr)
             (list 'cons cons)
             (list 'null null)
             (list 'map map)
             (list 'append append)
             (list 'list list)
             (list '+ +)
             (list '- -)
             (list '* *)
             (list '/ /)
             (list 'mod mod)
             (list '= =)
             (list '> >)
             (list '< <)))

(label apply-primitive-procedure
       (lambda (proc args)
         (apply (cdr proc) args)))

;;;
;;; Common actions.
;;;

;; Self-evaluating form.
(label self-evaluating-p
       (lambda (exp)
         (cond ((number exp) t)
               ((string exp) t)
               (t nil))))

;; Quote form.
(label quoted-p
       (lambda (exp)
         (tagged-list-p exp 'quote)))

;; Let form.
(label let-p
       (lambda (exp)
         (tagged-list-p exp 'let)))

(label let-to-combination
       (lambda (exp)
         (let ((assignment (let-assignment exp))
               (body (let-body exp)))
           (cons (make-lambda (let-variables assignment) body)
                 (let-values assignment)))))

(label let-values
       (lambda (exp)
         (if (null exp)
             nil
             (cons (car (cdr (car exp))) (let-values (cdr exp))))))

(label let-variables
       (lambda (exp)
         (if (null exp)
             nil
             (cons (car (car exp)) (let-variables (cdr exp))))))

(label let-body
       (lambda (exp)
         (list (car (cdr (cdr exp))))))

(label let-assignment
       (lambda (exp)
         (car (cdr exp))))

;; Lookup variable form.
(label variable-p
       (lambda (exp)
         (symbol exp)))

(label lookup-variable-value
       (lambda (var env)
         (label env-loop
                (lambda (env)
                  (label scan
                         (lambda (vars vals)
                           (cond ((null vars)
                                  (env-loop (cdr env)))
                                 ((eq var (car vars))
                                  (car vals))
                                 (t (scan (cdr vars) (cdr vals))))))
                  (if (eq env nil)
                      (error "Unbound variable")
                      (let ((frame (car env)))
                        (scan (car frame)
                              (cdr frame))))))
         (env-loop env)))

;; Assignment variable form.
(label assignment-p
       (lambda (exp)
         (tagged-list-p exp 'setq)))

(label eval-assignment
       (lambda (exp env)
         (set-variable-value (assignment-variable exp)
                             (meval (assignment-value exp) env)
                             env)
         'ok))

(label assignment-variable
       (lambda (exp)
         (car (cdr exp))))

(label assignment-value
       (lambda (exp)
         (car (cdr (cdr exp)))))

(label set-variable-value
       (lambda (var val env)
         (label env-loop
                (lambda (env)
                  (label scan
                         (lambda (vars vals)
                           (cond ((null vars)
                                  (env-loop (cdr env)))
                                 ((eq var (car vars))
                                  (rplaca vals val))
                                 (t (scan (cdr vars) (cdr vals))))))
                  (if (eq env nil)
                      (error "Unbound variable")
                      (let ((frame (car env)))
                        (scan (car frame)
                              (cdr frame))))))
         (env-loop env)))

;; Defintion variable form.
(label definition-p
       (lambda (exp)
         (tagged-list-p exp 'label)))

(label eval-definition
       (lambda (exp env)
         (define-variable
           (car (cdr exp))
           (meval (car (cdr (cdr exp))) env)
           env)
         'ok))
             
(label define-variable
       (lambda (var val env)
         (let ((frame (car env)))
           (label scan
                  (lambda (vars vals)
                    (cond ((null vars)
                           (add-binding-to-frame var val frame))
                          ((eq var (car vars))
                           (error "Variable already exists"))
                          (t (scan (cdr vars) (cdr vals))))))
           (scan (car frame)
                 (cdr frame)))))

(label add-binding-to-frame
       (lambda (var val frame)
         (rplaca frame (cons var (car frame)))
         (rplacd frame (cons val (cdr frame)))))

;; If form.
(label if-p
       (lambda (exp)
         (tagged-list-p exp 'if)))

(label eval-if
       (lambda (exp env)
         (if (meval (if-predicate exp) env)
             (meval (if-consequent exp) env)
             (meval (if-alternative exp) env))))

(label if-predicate
       (lambda (exp)
         (car (cdr exp))))

(label if-consequent
       (lambda (exp)
         (car (cdr (cdr exp)))))

(label if-alternative
       (lambda (exp)
         (if (not (null (cdr (cdr (cdr exp)))))
             (car (cdr (cdr (cdr exp))))
             'false)))

;; Lambda form.
(label lambda-p
       (lambda (exp)
         (tagged-list-p exp 'lambda)))

(label make-procedure
       (lambda (parameters body env)
         (list 'procedure parameters body env)))

(label lambda-parameters
       (lambda (exp)
         (car (cdr exp))))

(label lambda-body
       (lambda (exp)
         (cdr (cdr exp))))

(label make-lambda
       (lambda (parameters body)
         (cons 'lambda (cons parameters body))))

;; Progn form.
(label progn-p
       (lambda (exp)
         (tagged-list-p exp 'progn)))

(label eval-sequence
       (lambda (exps env)
         (cond ((null (cdr exps)) (meval (car exps) env))
               (t (meval (car exps) env)
                  (eval-sequence (cdr exps) env)))))

;; Cond form.
(label cond-p
       (lambda (exp)
         (tagged-list-p exp 'cond)))

(label cond-to-if
       (lambda (exp)
         (expand-clauses (cdr exp))))

(label expand-clauses
       (lambda (clauses)
         (if (null clauses)
             'false
             (let ((first (car clauses))
                   (rest (cdr clauses)))
               (list 'if
                     (car first)
                     (cond ((null (cdr first)) (cdr first))
                           ((null (cdr (cdr first))) (car (cdr first)))
                           (t (cons 'progn (cdr first))))
                     (expand-clauses rest))))))

;; Application form.
(label application-p
       (lambda (exp)
         (pair exp)))

(label list-of-values
       (lambda (exps env)
         (if (null exps)
             nil
             (cons (meval (car exps) env)
                   (list-of-values (cdr exps) env)))))

;; Init.
(label environment-init
       (lambda (env)
         (let ((initial-env
                (extend-environment
                 (map car primitive-procedures)
                 (map (lambda (proc) (cons 'primitive (car (cdr proc))))
                      primitive-procedures)
                 env)))
           (define-variable 't t initial-env)
           (define-variable 'nil nil initial-env)
           initial-env)))

;; The global environment.
(label environment (environment-init nil))
