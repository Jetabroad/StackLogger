StackLogger
===========

StackLogger is a proof-of-concept library that enables C# applications to log variables on the stack at the time of an exception. It has been used successfully with a ASP.NET application, but should not be considered production quality code. It might eat your children.

Currently, it can get the method argument values for a limited subset of types. It should be possible to extend this to more types, as well as local variable values, but this is not yet implemented.

Usage
=====

**Setup**

StackLogger works in both normal C# applications as well as ASP.NET. Here's an example of using it in ASP.NET:

```csharp
public class MvcApplication : System.Web.HttpApplication
{
    public static IStackLogger StackLogger { get; private set; }

    protected void Application_Start()
    {
        var installer = new StackLoggerInstaller();
        var physicalWebRootPath = Server.MapPath("~/");
        StackLogger = installer.InstallForWeb(physicalWebRootPath);
        StackLogger.Enabled = true;

        // typical ASP.NET application initialization here...
    }
}

```

**Getting Stack Details**

```csharp
var stackLogger = MvcApplication.StackLogger;
var frame = stackLogger.Data.Frames[0];
var methodName = frame.Name;

//param 0 is `this`
//param 1 is the first parameter to the function
var paramType = frame.Parameters[0].Type;
var paramValue = frame.Parameters[1].Value as IStringValue;
return $"method: {methodName}, paramType: {paramType}, paramValue: {paramValue.Value}";
```

Developer Details
=================

StackLogger is made up of two main components:

- Logger - This is written is C++, since it needs some low-level access to SEH and VEH (see [Technologies Used](#technologies-used)) in the Windows Kernel. Its purpose is to save the stack frame information whenever an exception is thrown.
- API. This is written is C#. It contains the interfaces and classes that are used to communicate with the Logger, such as query stack frames.

Technologies Used
=================

**Structured Exception Handling (SEH)**

Structured Exception Handling (SEH) is an exception system that is built into the Windows system, supported by the kernel. SEH is very powerful because we can catch hardware exceptions (e.g. segmentation fault, divide by zero) even in an application running in user mode. Visual C++ and CLR exception handling is built on top of SEH.

**Vectored Exception Handling (VEH)**

Vectored Exception Handling (VEH) is used to listen to SEH exceptions in the process before it's handled by the default SEH handler. We use it to listen to CLR exceptions and extract the information before it's raised to managed code.

**Component Object Model (COM)**

COM is a technology built into Windows that allow software that written in different language to communicate with each other. It's used for the C++ and C# interaction. We use the Active Template Library (ATL) and Windows Template Library (WTL) in Visual C++ to make COM easier to use.

**Data Access Component (DAC) for the CLR**

Data Access Component for the CLR is a special component of the .NET Framework that is used to access the code that is being executing by the CLR. It is a COM server. We use this to get the value of a method's argument in the call stack.


Library Design
==============

![Sequence Diagram](https://github.com/Jetabroad/StackLogger/raw/develop/Docs/StackLoggerSequenceDiagram.png)

![Class Diagram](https://github.com/Jetabroad/StackLogger/raw/develop/Docs/StackLoggerClassDiagram.png)
