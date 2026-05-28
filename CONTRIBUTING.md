# Contributing to Zlay

Zlay is a declarative UI and layout engine written in C/C++. The goal of this project is to provide a small, portable, and understandable UI/layout foundation that can be used with different rendering backends.

This project is still in early development, so the API, file structure, and internal design may change.

## Project Goals

Zlay focuses on:

- Simple and readable C/C++ code
- Portable layout and UI logic
- Minimal dependencies
- Clear separation between layout, rendering, and platform/backend code
- Friendly integration with C and C++ projects

## How to Contribute

You can contribute by:

- Reporting bugs
- Improving documentation
- Fixing build issues
- Adding examples
- Improving tests
- Suggesting API improvements
- Refactoring code without changing behavior

Before making a large change, please open an issue first so the direction can be discussed.

## Code Style

Please try to keep the code style consistent with the existing project.

General rules:

- Prefer simple C code over clever code
- Keep functions readable and focused
- Avoid unnecessary dependencies
- Use clear names for structs, functions, enums, and macros
- Keep platform-specific code separated when possible
- Do not introduce large architectural changes without discussion

## Pull Request Guidelines

Before submitting a pull request:

- Make sure the project builds successfully
- Keep the pull request focused on one topic
- Explain what the change does
- Mention any breaking changes
- Add or update examples when useful
- Update documentation if the public API changes

Small pull requests are easier to review than very large ones.

## Third-Party Code

Do not submit copied code from another project unless:

- The license is compatible with this project
- The original source is clearly credited
- The code is listed in `THIRD_PARTY_NOTICES.md` when required

If you are inspired by another library, please write the implementation yourself and mention the inspiration when relevant.

## AI-Assisted Contributions

AI tools may be used for brainstorming, refactoring, documentation, tests, or code review assistance.

However:

- You are responsible for any code you submit
- You must understand the submitted code
- Do not submit code copied from proprietary, leaked, or incompatible sources
- Do not submit AI-generated code if you are unsure about its origin or license compatibility
- All contributions must be submitted under this project's license

AI assistance is allowed, but responsibility stays with the contributor.

## License

By contributing to this project, you agree that your contribution will be licensed under the same license as the project.

Please check the `LICENSE` file before contributing.

## Respectful Discussion

Technical disagreement is allowed.

Personal attacks, harassment, insults, or hostile behavior are not welcome.

This project values calm, practical, and constructive discussion.
