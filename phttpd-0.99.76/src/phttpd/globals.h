/*
** globals.h
**
** Copyright (c) 1994-1995 Peter Eriksson <pen@signum.se>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifdef IN_MAIN_C
#define DCL(t,v,i)	t v = i
#else
#define DCL(t,v,i)	extern t v
#endif

DCL(int,extended_logging,0);
DCL(int,rkmultimode,0);
DCL(int,write_needs_auth, 1);
DCL(int,debug,0);
DCL(int,n_requests,0);
DCL(int,wait_mode,0);
DCL(int,no_copying_info,0);
DCL(int,listen_sock,-1);
DCL(int,keepalive_enabled,1);
DCL(int,keepalive_timeout,30);
DCL(int,keepalive_maxreq,4);
DCL(int,server_port,80);
DCL(int,bound_threads,0);
DCL(int,hostname_lookups,0);
DCL(int,ident_lookups,0);
DCL(uid_t,server_uid,60001);
DCL(gid_t,server_gid,60001);
DCL(int,concurrency,20);
DCL(int,n_listen,1024);
DCL(int,gc_time,2*60);
DCL(int,gc_sleep,1*60);
DCL(int,log_fd,-1);
DCL(int,terminate_flag,0);
DCL(int,maxurlsize,256*1024);


#ifndef INSTROOT
#  define INSTROOT "/opt/phttpd"
#endif

DCL(char *,server_home,INSTROOT "/db");
DCL(char *,modules_home,INSTROOT "/modules");
DCL(char *,user_dir,"pub");
DCL(char *,user_basedir,NULL);
DCL(char *,server_host,NULL);
DCL(char *,server_addr,"*");
DCL(char *,web_admin_name,NULL);
DCL(char *,web_admin_home,NULL);
DCL(char *,web_admin_email,NULL);
DCL(char *,default_file_handler,"file.so");
DCL(char *,default_dir_handler ,"dir.so");
DCL(char *,server_user,"nobody");
DCL(char *,server_group,"nobody");
DCL(char *,logfile_path,NULL);
DCL(char *,pidfile_path,NULL);
DCL(char *,cntfile_path,NULL);
DCL(char *,errfile_path,NULL);
DCL(char *,less_path,NULL);

DCL(char,server_url[2048],"<not set>");

DCL(time_t,start_time,0);




