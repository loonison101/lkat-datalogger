using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Serilog.Core;

namespace LKAT.Cmd
{
    //public class ApplicationStartupValidatorService
    //{
    //    private readonly List<IValidator> _validators;
    //    private readonly Logger _logger;

    //    public ApplicationStartupValidatorService(List<IValidator> validators, Logger logger)
    //    {
    //        _validators = validators;
    //        _logger = logger;
    //    }

    //    public int Validate()
    //    {
    //        var result = _validators.FirstOrDefault(x => x.Validate().Success == false);

    //        if (result == null)
    //        {
                
    //        }

    //        if (_validators.Any(x => x.Validate().Success == false))
    //        {
                
    //        }
    //        foreach (var validator in _validators)
    //        {
    //            var result = validator.Validate();
    //            if (!result)
    //        }
    //    }
    //}

    //public interface IValidator
    //{
    //    ValidatorResult Validate();
    //}

    //public class ValidatorResult
    //{
    //    public bool Success { get; set; }
    //    public bool Message { get; set; }
    //}


}
