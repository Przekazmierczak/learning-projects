CREATE INDEX "enrollments_student_id_index" ON "enrollments"("student_id", "course_id");
CREATE INDEX "enrollments_course_id_index" ON "enrollments"("course_id", "student_id");
CREATE INDEX "courses_department_index" ON "courses"("department", "number", "semester");
CREATE INDEX "courses_semester_index" ON "courses"("semester");
CREATE INDEX "satisfies_course_id_index" ON "satisfies"("course_id");