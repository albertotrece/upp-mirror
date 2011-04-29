topic "class TSignals";
[ $$0,0#00000000000000000000000000000000:Default]
[i448;a25;kKO9; $$1,0#37138531426314131252341829483380:structitem]
[l288;2 $$2,0#27521748481378242620020725143825:desc]
[0 $$3,0#96390100711032703541132217272105:end]
[H6;0 $$4,0#05600065144404261032431302351956:begin]
[i448;a25;kKO9;2 $$5,0#37138531426314131252341829483370:codeitem]
[l288;i1121;b17;O9;~~~.1408;2 $$6,0#10431211400427159095818037425705:param]
[{_}%EN-US 
[s1;:TSignals`:`:class:%- [@(0.0.255) class]_[* TSignals]&]
[s2; Class TSignals provides handling of Posix signals.&]
[s0; &]
[s2; Posix signals can be considered as events sent to the running 
process by kernel or other processes. Some of them are system 
signals that can`'t be handled by user applications; others can 
be useful to send inter`-process messages.&]
[s2; Apache web server, for examples, reacts to SIGHUP signal reloading 
configuration files without the need to restart the whole server.&]
[s0; &]
[s2; Signals are asynchronous events, as they can happen everytime 
during process execution; they pose the same kind of problems 
found in multithreading &]
[s0; &]
[s2; In order to simplify signal handling, and to avoid multithreading 
problems, the package translates the signal into an event queued 
to application event loop. User is then freed about synchronization 
problems but doing so the signal handling can only be used by 
gui apps in a synchronous fashion; you can`'t for example write 
a console application using this package.&]
[s0; &]
[s2; Signal handling is controlled by Signals() global function, 
which returns a reference to the global instance of TSignals 
class.&]
[s2; Two functions allow hooking of callbacks to the signal events 
: Handle() and Unhandle()&]
[s0; &]
[s2; Here a list of Posix signals, with description and default (unhandled) 
behaviour:&]
[s0; &]
[ {{1538:2606:5856<288;>288;h1; [s0;= [* Signal]]
:: [s0;= [* Default action]]
::t/42b/42 [s0;= [* Description]]
::t/15b/15 [s0; [@(226.85.0) SIGABRT]]
:: [s0; [2 Terminate process]]
:: [s0; [2 Process abort signal]]
:: [s0; [@(226.85.0) SIGALRM]]
:: [s0; [2 Terminate process]]
:: [s0; [2 Alarm clock]]
:: [s0; [@(226.85.0) SIGFPE]]
:: [s0; [2 Terminate process]]
:: [s0; [2 Erroneous arithmetic operation]]
:: [s0; [@(226.85.0) SIGHUP]]
:: [s0; [2 Terminate process]]
:: [s0; [2 Hangup]]
:: [s0; [@(226.85.0) SIGILL]]
:: [s0; [2 Terminate process]]
:: [s0; [2 Illegal instruction]]
:: [s0; [@(226.85.0) SIGINT]]
:: [s0; [2 Terminate process]]
:: [s0; [2 Terminal interrupt signal]]
:: [s0; [@(226.85.0) SIGKILL]]
:: [s0; [2 Terminate process]]
:: [s0; [2 Kill (cannot be caught or ignored)]]
:: [s0; [@(226.85.0) SIGPIPE]]
:: [s0; [2 Terminate process]]
:: [s0; [2 Write on a pipe with no one to read it]]
:: [s0; [@(226.85.0) SIGQUIT]]
:: [s0; [2 Terminate process]]
:: [s0; [2 Terminal quit signal]]
:: [s0; [@(226.85.0) SIGSEGV]]
:: [s0; [2 Terminate process]]
:: [s0; [2 Invalid memory reference]]
:: [s0; [@(226.85.0) SIGTERM]]
:: [s0; [2 Terminate process]]
:: [s0; [2 Termination signal]]
:: [s0; [@(226.85.0) SIGUSR1]]
:: [s0; [2 Terminate process]]
:: [s0; [2 User`-defined signal 1]]
:: [s0; [@(226.85.0) SIGUSR2]]
:: [s0; [2 Terminate process]]
:: [s0; [2 User`-defined signal 2]]
:: [s0; [@(226.85.0) SIGCHLD]]
:: [s0; [2 Ignore signal]]
:: [s0; [2 Child process terminated or stopped]]
:: [s0; [@(226.85.0) SIGCONT]]
:: [s0; [2 Resume process]]
:: [s0; [2 Continue executing, if stopped]]
:: [s0; [@(226.85.0) SIGSTOP]]
:: [s0; [2 Stop process]]
:: [s0; [2 Stop executing (cannot be caught)]]
:: [s0; [@(226.85.0) SIGTSTP]]
:: [s0; [2 Stop process]]
:: [s0; [2 Terminal stop signal]]
:: [s0; [@(226.85.0) SIGTTIN]]
:: [s0; [2 Stop process]]
:: [s0; [2 Background process attempting read]]
:: [s0; [@(226.85.0) SIGTTOU]]
:: [s0; [2 Stop process]]
:: [s0; [2 Background process attempting write]]
:: [s0; [@(226.85.0) SIGBUS]]
:: [s0; [2 Terminate process]]
:: [s0; [2 Access to an undefined memory]]
:: [s0; [@(226.85.0) SIGPOLL]]
:: [s0; [2 Terminate process]]
:: [s0; [2 Pollable event]]
:: [s0; [@(226.85.0) SIGPROF]]
:: [s0; [2 Terminate process]]
:: [s0; [2 Profiling timer expired]]
:: [s0; [@(226.85.0) SIGSYS]]
:: [s0; [2 Terminate process]]
:: [s0; [2 Bad system call]]
:: [s0; [@(226.85.0) SIGTRAP]]
:: [s0; [2 Terminate process]]
:: [s0; [2 Trace/breakpoint trap]]
:: [s0; [@(226.85.0) SIGURG]]
:: [s0; [2 Ignore signal]]
:: [s0; [2 High bandwidth data is available at a socket]]
:: [s0; [@(226.85.0) SIGVTALRM]]
:: [s0; [2 Terminate process]]
:: [s0; [2 Virtual timer expired]]
:: [s0; [@(226.85.0) SIGXCPU]]
:: [s0; [2 Terminate process]]
:: [s0; [2 CPU time limit exceeded]]
:: [s0; [@(226.85.0) SIGXFSZ]]
:: [s0; [2 Terminate process]]
:: [s0; [2 File size limit exceeded]]}}&]
[s3; &]
[s4;%- &]
[s5;:Signals`(void`):%- [_^TSignals^ TSignals]_`&[* Signals]([@(0.0.255) void])&]
[s2; &]
[s2; Returns global TSignals object  Used to acceass all package 
functions&]
[s3; &]
[s4;%- &]
[s5;:TSignals`:`:Handle`(int`,Callback const`&`):%- [@(0.0.255) bool]_[* Handle]([@(0.0.255) i
nt]_[*@3 sig], [_^Callback^ Callback]_[@(0.0.255) const]_`&[*@3 cb])&]
[s0;%- &]
[s2; Installs a Callback [%-*@3 cb] to handle [%-*@3 sig] Posix signal&]
[s0; &]
[s6; [*/ Return value]-|-|boolean, true if the call succeeded&]
[s0; &]
[s2; Example usage:&]
[s0; &]
[s2; -|-|Signals().Handle(SIGHUP, THISBACK(onHup));&]
[s3; &]
[s4;%- &]
[s5;:TSignals`:`:Unhandle`(int`):%- [@(0.0.255) bool]_[* Unhandle]([@(0.0.255) int]_[*@3 sig])
&]
[s0;%- &]
[s2; Removes the handler for Posix [%-*@3 sig] signal&]
[s0; &]
[s6; [*/ Return value]-|boolean, true if the call succeeded&]
[s0; &]
[s2; Example usage:&]
[s0; &]
[s2; -|-|Signals().Unhandle(SIGHUP);&]
[s3; &]
[s4;%- &]
[s5;:TSignals`:`:StoreInstance`(void`):%- [@(0.0.255) bool]_[* StoreInstance]([@(0.0.255) v
oid])&]
[s0; &]
[s2; Stores current process info on file allowing check for other 
instances&]
[s0;%- &]
[s6; [*/ Return value]-|boolean, true if the call succeeded&]
[s0; &]
[s2; Example usage:&]
[s0; &]
[s2; -|-|Signals().StoreInstance();&]
[s3; &]
[s4;%- &]
[s5;:TSignals`:`:IsOtherInstanceRunning`(void`):%- [@(0.0.255) bool]_[* IsOtherInstanceRu
nning]([@(0.0.255) void])&]
[s0; &]
[s2; Checks wether another instance of this app is already running&]
[s0;%- &]
[s6; [*/ Return value]-|boolean, true if another instance is running&]
[s0; &]
[s2; Example usage:&]
[s0; &]
[s2; -|-|Signals().IsOtherInstanceRunning();&]
[s3;%- &]
[s4;%- &]
[s5;:TSignals`:`:Send`(int`,int`):%- [@(0.0.255) bool]_[* Send]([@(0.0.255) int]_[*@3 sig], 
[@(0.0.255) int]_[*@3 procId])&]
[s0; &]
[s2;%- [%% Sends a ][*@3 sig][%%  Posix signal to the process identified 
by ][*@3 procId]&]
[s0; &]
[s2; Example usage:&]
[s0; &]
[s2; -|-|Signals().Send(SIGHUP, myProcessId);&]
[s3; &]
[s4;%- &]
[s5;:TSignals`:`:Send`(int`):%- [@(0.0.255) bool]_[* Send]([@(0.0.255) int]_[*@3 sig])&]
[s0; &]
[s2; Sends Posix signal [%-*@3 sig] to another running instance of 
this application&]
[s0; &]
[s2; Example usage:&]
[s0; &]
[s2; -|-|Signals().Send(SIGHUP);&]
[s3; ]