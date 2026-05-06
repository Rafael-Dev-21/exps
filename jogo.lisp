;; Tsk, Tsk,  chatgpt não sabe nem fazer jogo direito 

(defparameter *nodes* '((floresta (Você está em um denso matagal.
                                   Árvores de alta copa cobrem o sol e folhas podres jazem ao chão.))
                        (campo    (Você está num campanado aberto.
                                  Grama alta até os joelhos.))))

(defparameter *edges* '((floresta (campo estrada leste) (floresta estrada norte))
                        (campo    (floresta estrada oeste))))

(defparameter *player-location* 'floresta)
; Eu odeio mecânicas de fomes e mecânicas de punição
(defparameter *player-health* 31)

(defun describe-location (nodes location)
  (cadr (assoc location nodes)))

(defun describe-edge (edge)
  `(tem uma ,(cadr edge) para o ,(caddr edge) daqui.))

(defun describe-edges (edges location)
  (apply #'append (mapcar #'describe-edge (cdr (assoc location edges)))))

(defun olhar ()
  (append (describe-location *nodes* *player-location*)
          (describe-edges    *edges* *player-location*)))

(defun ir (where)
  (let* ((edges (cdr (assoc *player-location* *edges*)))
         (edge  (find-if #'(lambda (x) (member where x)) edges)))
    (if edge
      (progn
        (setf *player-location* (car edge))
        (olhar))
      `(Não há caminho para ,where daqui.))))

(defparameter *allowed* '(olhar ir))

(defun game-read ()
  (let ((cmd (read-from-string
               (concatenate 'string "(" (read-line) ")"))))
    (flet ((quote-it (x)
             (list 'quote x)))
      (cons (car cmd) (mapcar #'quote-it (cdr cmd))))))

(defun game-eval (sexp)
  (if (member (car sexp) *allowed*)
    (eval sexp)
    '(Eu não conheço esse comando.)))

(defun tweak-text (lst caps lit)
  (when lst
    (let ((item (car lst))
          (rest (cdr lst)))
      (cond ((eql item #\space) (cons item (tweak-text rest caps lit)))
            ((member item '(#\! #\? #\.)) (cons item (tweak-text rest t lit)))
            ((eql item #\") (tweak-text rest caps (not lit)))
            (lit (cons item (tweak-text rest nil lit)))
            (caps (cons (char-upcase item) (tweak-text rest nil lit)))
            (t (cons (char-downcase item) (tweak-text rest nil nil)))))))

(defun game-print (lst)
  (princ (coerce (tweak-text (coerce
                               (string-trim "() "
                                            (prin1-to-string lst))
                               'list)
                             t
                             nil)
                 'string))
  (fresh-line))

(defun game-repl ()
  (let ((cmd (game-read)))
    (unless (eq (car cmd) 'sair)
      (game-print (game-eval cmd))
      (game-repl))))
