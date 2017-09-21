#include "error.hpp"

/*===========================================================================================*/
/*===============================      ERRORCONTEXT      ====================================*/
/*===========================================================================================*/

ErrorContext::ErrorContext(std::string const& contextName) {
	_contextName = contextName;
}

ErrorContext::ErrorContext(ErrorContext const& copy) {
	_contextName = copy._contextName;
	_errors = copy._errors;
	_warnings = copy._warnings;
}


void ErrorContext::addError(std::string const& message) {
	const struct ErrorContext::basicError currentError = {
		_contextName,
		message
	};
	_errors.push_back(currentError);
}

void ErrorContext::addErrors(std::vector<std::string> const& messages) {
	for(auto& i : messages) {		
		struct ErrorContext::basicError currentError = {
			_contextName,
			i
		};

		_errors.push_back(currentError);
	}
}

void ErrorContext::addErrors(std::vector<struct ErrorContext::basicError> const& errors) {
	for(auto& i : errors) {
		// format the name as parentContext > addedContext
		std::string name;
		if(_contextName.size() != 0) {
			if(i.contextName.size() != 0) {
				name = _contextName + " > " + i.contextName;
			}
			else {
				name = _contextName;
			}
		}
		else if(i.contextName.size() != 0) {
			name = i.contextName;
		}
		struct ErrorContext::basicError currentError = {
			name,
			i.message
		};

		_errors.push_back(currentError);
	}
}

void ErrorContext::addWarning(std::string const& message) {
	const struct ErrorContext::basicError currentError = {
		_contextName,
		message
	};
	_warnings.push_back(currentError);
}

void ErrorContext::addWarnings(std::vector<std::string> const& messages) {
	for(auto& i : messages) {		
		struct ErrorContext::basicError currentError = {
			_contextName,
			i
		};

		_warnings.push_back(currentError);
	}
}

void ErrorContext::addWarnings(std::vector<struct ErrorContext::basicError> const& warnings) {
	for(auto& i : warnings) {
		std::string name;
		if(_contextName.size() != 0) {
			if(i.contextName.size() != 0) {
				name = _contextName + " > " + i.contextName;
			}
			else {
				name = _contextName;
			}
		}
		else if(i.contextName.size() != 0) {
			name = i.contextName;
		}
		struct ErrorContext::basicError currentError = {
			name,
			i.message
		};
		
		_warnings.push_back(currentError);
	}
}


std::vector<struct ErrorContext::basicError> const& ErrorContext::getErrors() const {
	return _errors;
}

std::vector<struct ErrorContext::basicError> const& ErrorContext::getWarnings() const {
	return _warnings;
}

bool ErrorContext::hasErrors() const {
	return !_errors.empty();	
}

bool ErrorContext::hasWarnings() const {
	return !_warnings.empty();
}

/*===========================================================================================*/
/*===================================      ERROR      =======================================*/
/*===========================================================================================*/

Error::Error(std::string const& contextName) {
	_contexts.push_back(ErrorContext(contextName));
	_top = &_contexts[0];
}

void Error::addError(std::string const& errorMessage) {
	_top->addError(errorMessage);
}

void Error::addErrors(std::vector<std::string> const& errorMessages) {
	_top->addErrors(errorMessages);
}

void Error::addWarning(std::string const& warningMessage) {
	_top->addWarning(warningMessage);
}

void Error::addWarnings(std::vector<std::string> const& warningMessages) {
	_top->addWarnings(warningMessages);
}

ErrorContext const& Error::getCurrentContext() const {
	return *_top;
}

ErrorContext Error::pop() {
	ErrorContext ec = *_top;

	// if it was the last context, create a new one.
	if(_contexts.size() == 1) {
		_contexts.pop_back();
		_contexts.push_back(ErrorContext());
		_top = &_contexts[0];

		return ec;
	}
	else {
		_contexts.pop_back();
		_top = &_contexts[_contexts.size() - 1];
	}
	return ec;
}

void Error::push(Error const& error) {
	Error copy = error;

	while(copy._contexts.size() != 1) {
		copy.fold();
	}

	push();
	_top->addErrors(copy._top->getErrors());
	_top->addWarnings(copy._top->getWarnings());
}

void Error::push(std::string const& contextName) {
	_contexts.push_back(ErrorContext(contextName));
	_top = &_contexts[_contexts.size() - 1];
}

void Error::fold() {
	ErrorContext ec(pop());
	_top->addErrors(ec.getErrors());
	_top->addWarnings(ec.getWarnings());
}

void Error::foldToError() {
	ErrorContext ec = pop();
	_top->addErrors(ec.getErrors());
	_top->addErrors(ec.getWarnings());
}

void Error::foldToWarning() {
	ErrorContext ec(pop());
	_top->addWarnings(ec.getErrors());
	_top->addWarnings(ec.getWarnings());
}

bool Error::hasErrors() const {
	return _top->hasErrors();
}

bool Error::hasWarnings() const {
	return _top->hasWarnings();
}

void Error::clear(std::string const& name) {
	_contexts = std::vector<ErrorContext>();
	_contexts.push_back(ErrorContext(name));
	_top = &_contexts[0];
}

void Error::writeTo(std::ostream & stream) const {
	for(auto& i : _top->getErrors()) {
		stream << "Error : [" << i.contextName << "] : " << i.message << std::endl;
	}

	for(auto& i : _top->getWarnings()) {
		stream << "Warning : [" << i.contextName << "] : " << i.message << std::endl;
	}
}

void Error::writeWarningsTo(std::ostream & stream) const {

	for(auto& i : _top->getWarnings()) {
		stream << "Warning : [" << i.contextName << "] : " << i.message << std::endl;
	}
}

void Error::writeErrorsTo(std::ostream & stream) const {
	for(auto& i : _top->getErrors()) {
		stream << "Error : [" << i.contextName << "] : " << i.message << std::endl;
	}
}