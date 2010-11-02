Patches for operator overloading support in Java language.
Support JavaC compiler, Eclipse Compiler for Java and Eclipse IDE.

See Test.java for example.

langtools.patch
 for javac compiler (OpenJDK 6 and 7)
 sources: $ hg clone http://hg.openjdk.java.net/jdk6/jdk6/langtools

org.eclipse.jdt.core.patch
 for Eclipse Compiler for Java (ecj) and Eclipse IDE. Tested on v3.6.1.
 sources: $ cvs -d ':pserver:anonymous@dev.eclipse.org/cvsroot/eclipse' co -r R3_6_maintenance  org.eclipse.jdt.core

See also http://github.com/amelentev/lombok
