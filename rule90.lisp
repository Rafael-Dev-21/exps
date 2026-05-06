(defun rot (n list)
  (cond ((zerop n) list)
        ((< n 0)
         (rot (+ (length list) n) list))
        (t (append (nthcdr n list)
                   (butlast list
                            (- (length list)
                               n))))))

(defun rule90 (list)
  (let ((left-rot  (rot 1 (copy-list list)))
        (right-rot (rot -1 (copy-list list))))
    (loop for val1 in left-rot
      for val2 in right-rot
      for num = (+ val1 val2)
      collect (if (= num 1) 1 0))))

(defparameter *r*
  (loop for i from 1 to 7
    collect (if (member i '(3 4 5)) 1 0)))

(format t "~{~a~%~}"
        (let ((r *r*))
          (loop for i from 1 to 24
            collect r
            do (setf r (rule90 r)))))
