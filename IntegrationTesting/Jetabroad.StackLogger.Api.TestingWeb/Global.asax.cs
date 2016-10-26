using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;
using System.Web.Routing;

namespace Jetabroad.StackLogger.Api.TestingWeb
{
    public class MvcApplication : System.Web.HttpApplication
    {

        public static IStackLogger stackLogger { get; private set; }

        protected void Application_Start()
        {
            var installer = new StackLoggerInstaller();
            //get root path of web
            var physicalWebRootPath = Server.MapPath("~/");
            stackLogger = installer.InstallForWeb(physicalWebRootPath);
            stackLogger.Enabled = true;
            AreaRegistration.RegisterAllAreas();
            RouteConfig.RegisterRoutes(RouteTable.Routes);
        }
    }
}
