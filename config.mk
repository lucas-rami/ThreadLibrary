###########################################################################
# This is the include file for the make file.
###########################################################################
# You should have to edit only this file to get things to build.
#

###########################################################################
# Tab stops
###########################################################################
# If you use tabstops set to something other than the international
# standard of eight characters, this is your opportunity to inform
# our print scripts.
TABSTOP = 8

###########################################################################
# The method for acquiring project updates.
###########################################################################
# This should be "afs" for any Andrew machine, "web" for non-andrew machines
# and "offline" for machines with no network access.
#
# "offline" is strongly not recommended as you may miss important project
# updates.
#
UPDATE_METHOD = afs

###########################################################################
# WARNING: Do not put extraneous test programs into the REQPROGS variables.
#          Doing so will put your grader in a bad mood which is likely to
#          make him or her less forgiving for other issues.
###########################################################################

###########################################################################
# Mandatory programs whose source is provided by course staff
###########################################################################
# A list of the programs in 410user/progs which are provided in source
# form and NECESSARY FOR THE KERNEL TO RUN
#
# The idle process is a really good thing to keep here.
#
410REQPROGS = idle

###########################################################################
# Mandatory programs provided in binary form by course staff
###########################################################################
# A list of the programs in 410user/progs which are provided in binary
# form and NECESSARY FOR THE KERNEL TO RUN
#
# You may move these to 410REQPROGS to test your syscall stubs once
# they exist, but we will grade you against the binary versions.
# This should be fine.
#
410REQBINPROGS = shell init

###########################################################################
# WARNING: When we test your code, the two TESTS variables below will be
# ignored.  Your kernel MUST RUN WITHOUT THEM.
###########################################################################

###########################################################################
# Test programs provided by course staff you wish to run
###########################################################################
# A list of the test programs you want compiled in from the 410user/progs
# directory
#
410TESTS = startle actual_wait cat getpid_test1 halt_test misbehave misbehave_wrap nibbles stack_test1 wild_test1 agility_drill cyclone racer beady_test bistromath cvar_test excellent join_specific_test juggle largetest mandelbrot multitest mutex_destroy_test paraguay rwlock_downgrade_read_test switzerland thr_exit_join

###########################################################################
# Test programs you have written which you wish to run
###########################################################################
# A list of the test programs you want compiled in from the user/progs
# directory
#
STUDENTTESTS = print_test fork_test

###########################################################################
# Object files for your thread library
###########################################################################
THREAD_OBJS = malloc.o panic.o mutex.o atomic_ops.o cond_var.o queue.o linked_list.o hash_table.o thr_create.o thread_fork.o thr_init.o thr_exit.o thr_join.o tcb.o get_esp.o thr_getid.o thr_yield.o sem.o rwlock.o rwlock_helper.o mutex_asm.o

# Thread Group Library Support.
#
# Since libthrgrp.a depends on your thread library, the "buildable blank
# P2" we give you can't build libthrgrp.a.  Once you set up your thread
# library and fix THREAD_OBJS above, uncomment this line to enable building
# libthrgrp.a:
410USER_LIBS_EARLY += libthrgrp.a

###########################################################################
# Object files for your syscall wrappers
###########################################################################
SYSCALL_OBJS = vanish.o set_status.o print.o deschedule.o exec.o fork.o getchar.o gettid.o make_runnable.o readline.o sleep.o swexn.o wait.o yield.o set_term_color.o get_cursor_pos.o set_cursor_pos.o halt.o readfile.o task_vanish.o new_pages.o remove_pages.o get_ticks.o misbehave.o

###########################################################################
# Object files for your automatic stack handling
###########################################################################
AUTOSTACK_OBJS = autostack.o page_fault_handler.o

###########################################################################
# Data files provided by course staff to be included in the RAM disk
###########################################################################
# A list of the data files you want copied in from the 410user/files
# directory (see 410user/progs/cat.c)
#
410FILES =

###########################################################################
# Data files you provide to be included in the RAM disk
###########################################################################
# A list of the data files you want copied in from the user/files
# directory (see 410user/progs/cat.c)
#
STUDENTFILES =
