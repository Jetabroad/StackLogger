// Copyright (c) 2018 Jetabroad (Thailand) Co., Ltd. All Rights Reserved.
// Licensed under the MIT license. See the LICENSE.md file in the project root for license information.
using System;
using System.Web.Mvc;

namespace Jetabroad.StackLogger.Api.TestingWeb.Controllers
{
        
    public class HomeController:Controller
    {

        public ActionResult Index()
        {
            try
            {
                MethodThatThrowException("Hello World");
            }
            catch
            {

            }

            var stackLogger = MvcApplication.stackLogger;
            var frame = stackLogger.Data.Frames[0];
            var methodName = frame.Name;
            var paramType = frame.Parameters[0].Type;

            //param 0 is this (HomeController)
            //param 1 is string
            var paramValue = frame.Parameters[1].Value as IStringValue;
            return Content($"method: {methodName}, paramType: {paramType}, paramValue: {paramValue.Value}");
        }

        private  void MethodThatThrowException(string stringParam)
        {
            throw new Exception("generic exception");
        }


    }
}