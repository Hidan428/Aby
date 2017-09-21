#ifndef _ERROR_HPP_
#define _ERROR_HPP_

#include <iostream>
#include <string>
#include <utility>
#include <vector>

/**
*	Defines an error context. Error contextes can be stacked on top of each other, and merged together, from top to bottom.
*	Warnings and errors can be attached to a context, allowing function or code blocks to group errors using a named context.
*	When merging 2 contextes, the top one is fused into the bottom one, which keeps track of the grouping (names associated with
*	the contextes are not lost).
*	For example :
*	Context1 : (name: root)
*		- An error message
*		- another error message
*	pushed on top of context1 comes context2 : (name : function)
*		- an error inside the pushed context
*	The two are merged (context2 on context1). Context1 is now :
*	Context1 : (name: root)
*	[root] : An error message
*	[root] : another error message
*	[root > function] an error inside the pushed context
*/
class ErrorContext {
	public:
		struct basicError {
			std::string contextName;
			std::string message;
		};
	protected:
		std::string _contextName;

		std::vector<struct ErrorContext::basicError> _errors;
		std::vector<struct basicError> _warnings;

	public:
		/**
		*	Creates a new error context.
		*	@param[in] name - The name to give this context.
		*/
		ErrorContext(std::string const& name = "");

		ErrorContext(ErrorContext const& copy);

		/**
		*	Adds an error message to this context.
		*	@param[in] message - The message to add.
		*/
		void addError(std::string const& message);

		/**
		*	Adds multiple error messages to this context.
		*	@param[in] messages - The messages to add.
		*/
		void addErrors(std::vector<std::string> const& messages);

		/**
		*	Adds multiple errors to this context.
		*	@param[in] errors - The errors to add.
		*/
		void addErrors(std::vector<struct basicError> const& errors);

		/**
		*	Adds a warning message to this context.
		*	@param[in] message - The warning message to add.
		*/
		void addWarning(std::string const& message);

		/**
		*	Adds multiple warning messages to this context.
		*	@param[in] messages - The messages to add.
		*/
		void addWarnings(std::vector<std::string> const& messages);

		/**
		*	Adds multiple warnings to this context.
		*	@param[in] warnings - The warnings to add.
		*/
		void addWarnings(std::vector<struct basicError> const& warnings);

		/**
		*	Returns all the errors attached to this context.
		*/
		std::vector<struct ErrorContext::basicError> const& getErrors() const;

		/**
		*	Returns all the warnings attached to this context.
		*/
		std::vector<struct ErrorContext::basicError> const& getWarnings() const;

		/**
		*	Returns whether this context contains errors or not.
		*/
		bool hasErrors() const;

		/**
		*	Returns whether this context contains warnings or not.
		*/
		bool hasWarnings() const;
};

/**
*	Acts as a container for multiple ErrorContext, manage the pushing and merging of the errors and warnings.
*/
class Error {
	protected:
		std::vector<ErrorContext> _contexts;
		ErrorContext* _top;

	public:
		/**
		*	Creates a new error handler.
		*	@param[in] contextName - The name to give to the root context of this handler.
		*/
		Error(std::string const& contextName = "");

		/**
		*	Add a new context that contains all the errors and warnings of the parameter error. If error contains more than one context,
		*	all its contextes are folded until only one remains.
		*/
		void push(Error const& error);

		/**
		*	Adds an error to the top context of this handler.
		*	@param[in] errorMessage - The message to add.
		*/
		void addError(std::string const& errorMessage);

		/**
		*	Adds multiple errors to the top context of this handler.
		*	@param[in] errorMessages - The messages to add.
		*/
		void addErrors(std::vector<std::string> const& errorMessages);

		/**
		*	Adds a warning to the top context of this handler.
		*	@param[in] warningMessage - The warning to add.
		*/
		void addWarning(std::string const& warningMessage);

		/**
		*	Adds multiple warnings to the top context of this handler.
		*	@param[in] warningMessages - The messages to add.
		*/
		void addWarnings(std::vector<std::string> const& warningMessages);

		/**
		*	Returns the top context of this handler.
		*/
		ErrorContext const& getCurrentContext() const;

		/**
		*	Deletes the top context of this handler. If it is the last handler, another one is created right after the deletion.
		*	@returns The deleted context.
		*/
		ErrorContext pop();

		/**
		*	Add a new context on top of this handler.
		*	@param[in] contextName - The name to give to the new context.
		*/
		void push(std::string const& contextName = "");

		/**
		*	Merges the top context with the one under, then, deletes the top context.
		*/
		void fold();

		/**
		*	Merges the top context with the one under, then, deletes the top context. The warnings are converted to errors.
		*/
		void foldToError();

		/**
		*	Merges the top contet with the one under, then, deletes the top context. The errors are converted to warnings.
		*/
		void foldToWarning();

		/**
		*	Returns whether the top context contains errors or not.
		*/
		bool hasErrors() const;

		/**
		*	Returns whether the top context contains warnings or not.
		*/
		bool hasWarnings() const;

		/**
		*	Removes all the errors and warnings and delete all the contexts except one.
		*	@param[in] name - The name of the context.
		*/
		void clear(std::string const& name = "");

		/**
		*	Write the errors and the warnings of the current context to a stream.
		*	@param[in] stream
		*/
		void writeTo(std::ostream & stream) const;

		/**
		*	Write only the warnings of the current context to a stream.
		*	@param[in] stream
		*/
		void writeWarningsTo(std::ostream & stream) const;

		/**
		*	Write only the errors of the current context to a stream.
		*	@param[in] stream
		*/
		void writeErrorsTo(std::ostream & stream) const;
};

#endif