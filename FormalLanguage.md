# Description #
## Running example ##
To demonstrate how to use formalism the next upward fractal zigzag pattern would be used:

![http://fractal-analysis.googlecode.com/svn/wiki/images/fractalzigzag.png](http://fractal-analysis.googlecode.com/svn/wiki/images/fractalzigzag.png)

It consists of three parts: up-down-up, where first and last "up" parts are upward zigzags too and middle "down" part is the downward zigzag.

## Basics ##
Each pattern have following form:
```
	Pattern = description @ guard;
```

**Description** part express structure of the pattern (like "up-down-up" in example). It is based on reduced [Extended Backus-Naur Form (EBNF)](http://en.wikipedia.org/wiki/Extended_Backus-Naur_Form). In terms of EBNF it could include three kinds of symbols:
  * **nonterminals** which are names of same or different pattern that are structural element of this pattern in a given position.
  * **terminals** which are names of predefined simple linear structural elements. They are the lowest level elements that extracted from time series in any way.
  * EBNF special symbols. Supported symbols are: concatenation (`... whitespace ...`), alternative (`... | ...`), option (`[ ... ]`) and grouping (`(...)`).
Also each symbol have unique (within this pattern) number associated with it in the way to distinguish symbols from each other in gurad part.

For description of example the two terminal symbols would be used: `up`, which means two adjacent points of time series first lower that second, and `down`, which is the contrary. More formal: if `a = (x1, y1)` and `b = (x2, y1)` are two points of time series X, than segment `(a, b)` would be marked one of symbols `up` or `down` according to rule:
```
	up    if y2 >= y1;
	down  otherwise.
```

To describe upward fractal zigzag we need these patterns (guards will be defined later):
```
	Zigzag     = UpZigzag_1 DownZigzag_2 UpZigzag_3 @ guard;
	UpZigzag   = (up_1   down_2 up_3  ) | (UpZigzag_1   DownZigzag_2 UpZigzag_3  ) @ guard;
	DownZigzag = (down_1 up_2   down_3) | (DownZigzag_1 UpZigzag_2   DownZigzag_3) @ guard;
```

**Guard** part express limitations of the description's symbols (like maximum size, minimal duration etc.). It based on simple formal language that include functional, math and logical symbols. In fact it's predicate of description, boolean function that served to determinate if specified sequence of symbols that have same structure as description of pattern, is the pattern.

For example, this guard means "maximum of the symbol `A` is greater than minimum of symbol `B` mult by 0.1":
```
	max(A) >= min(B)*0.1
```

Zigzag example has following feature that can not be defined in description part:

![http://fractal-analysis.googlecode.com/svn/wiki/images/fractalzigzag_features.png](http://fractal-analysis.googlecode.com/svn/wiki/images/fractalzigzag_features.png)

These features can be expressed as follow:
```
	Zigzag     = UpZigzag_1 DownZigzag_2 UpZigzag_3 @ 
				(min(UpZigzag_1) < min(DownZigzag_2)) and (max(UpZigzag_3) > max(DownZigzag_2));

	UpZigzag   = up_1 down_2 up_3 @
				(min(up_1) < min(down_2)) and (max(up_3) > max(down_2));

	UpZigzag   = UpZigzag_1 DownZigzag_2 UpZigzag_3 @
				(min(UpZigzag_1) < min(DownZigzag_2)) and (max(UpZigzag_3) > max(DownZigzag_2));

	DownZigzag = down_1 up_2 down_3 @ 
				(max(down_1) > max(up_2)) and (min(down_3) < min(up_2));

	DownZigzag = DownZigzag_1 UpZigzag_2 DownZigzag_3 @
				(max(DownZigzag_1) > max(UpZigzag_2)) and (min(DownZigzag_3) < min(UpZigzag_2));
```

`UpZigzag` and `DownZigzag` have two declarations - it is the same as alternative, but guards are different for alternatives so it was splited.
This example describe fractal zigzag of any nesting level.

# EBNF Syntax #
Syntax is not completed and may change over time.

## Description ##

```
  name         = char {char | digit};
  indexed      = name "_" number;
  ebnf         = name "=" expr;
  expr         = term  {"|" term};
  term         = factor >> {factor};
  factor       = indexed | "(" expr ")" | "[" expr "]";
```

## Guard ##
```
  S            = b_expr;
  b_expr       = b_term ("or" b_term)*
  b_term       = b_not ("and" b_not)*
  b_not        = ["not"] b_factor
  b_factor     = relation
  relation     = m_expr [relop m_expr]
  m_expr       = m_term (("+" | "-") m_term)*
  m_term       = m_sig_factor (("*" | "/") m_factor)*
  m_sig_factor = ["+" | "-"] m_factor
  m_factor     = if_statement | function | b_const | m_const | "(" b_expr ")"
  if_statement   = "if" b_expr "then" b_expr ["else" b_expr]
  function     = name "(" arg ("," arg)* ")"
  b_const      = "true" | "false"
  m_const      = double_number;
  m_var        = indexed | name;
  relop       %= "<=" | ">=" | "<" | ">" | "==" | "!="
  arg          = if_statement | function | m_const | b_const | indexed | name
  name        %= alpha >> *alnum
  indexed     %= name '_' (digit+ | "*");
```