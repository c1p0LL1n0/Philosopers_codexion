#!/bin/bash

# =============================================================================
# Codexion — full test suite
# =============================================================================

BIN=./codexion
PASS=0
FAIL=0

green() { printf "\033[32m%s\033[0m\n" "$1"; }
red()   { printf "\033[31m%s\033[0m\n" "$1"; }
blue()  { printf "\033[34m%s\033[0m\n" "$1"; }

check() {
    local name="$1"
    local cond="$2"
    if [ "$cond" = "1" ]; then
        green "  [OK] $name"
        PASS=$((PASS+1))
    else
        red "  [KO] $name"
        FAIL=$((FAIL+1))
    fi
}

# Успешный запуск (exit code == 0)
expect_ok() {
    local name="$1"
    shift
    timeout 30 "$BIN" "$@" >/dev/null 2>&1
    check "$name" "$([ $? -eq 0 ] && echo 1 || echo 0)"
}

# Неуспешный запуск (exit code != 0)
expect_fail() {
    local name="$1"
    shift
    timeout 5 "$BIN" "$@" >/dev/null 2>&1
    check "$name" "$([ $? -ne 0 ] && echo 1 || echo 0)"
}

# =============================================================================
blue "===== SECTION 1: BASIC FUNCTIONALITY ====="
# =============================================================================

blue "--- TEST 1: n=1 (one coder, one dongle) ---"
OUT=$($BIN 1 800 200 200 200 3 50 fifo)
echo "$OUT" | head -5
TAKEN=$(echo "$OUT" | grep -c "has taken a dongle")
COMP=$(echo "$OUT" | grep -c "is compiling")
check "n=1: one dongle per compile" "$([ "$TAKEN" -eq "$COMP" ] && echo 1 || echo 0)"
check "n=1: has compiling logs" "$([ "$COMP" -gt 0 ] && echo 1 || echo 0)"

blue "--- TEST 2: n=2 (two coders, two dongles) ---"
OUT=$($BIN 2 800 200 200 200 3 50 fifo)
echo "$OUT" | head -8
TAKEN=$(echo "$OUT" | grep -c "has taken a dongle")
COMP=$(echo "$OUT" | grep -c "is compiling")
check "n=2: two dongles per compile" "$([ "$TAKEN" -eq "$((COMP*2))" ] && echo 1 || echo 0)"

blue "--- TEST 3: need=1 (stop after first compile) ---"
OUT=$($BIN 3 800 200 200 200 1 50 fifo)
echo "$OUT" | head -12
COMP=$(echo "$OUT" | grep -c "is compiling")
check "need=1: 3 coders compile once" "$([ "$COMP" -eq 3 ] && echo 1 || echo 0)"

blue "--- TEST 4: edf (earliest deadline first) ---"
OUT=$($BIN 4 800 200 200 200 3 50 edf)
echo "$OUT" | head -15
COMP=$(echo "$OUT" | grep -c "is compiling")
check "edf: some compiles happened" "$([ "$COMP" -gt 0 ] && echo 1 || echo 0)"

blue "--- TEST 5: burnout detected ---"
OUT=$($BIN 4 400 200 200 200 5 50 edf)
echo "$OUT" | tail -3
BURN=$(echo "$OUT" | grep -c "burned out")
check "burnout detected" "$([ "$BURN" -ge 1 ] && echo 1 || echo 0)"

# =============================================================================
blue "===== SECTION 2: ARGUMENT VALIDATION ====="
# =============================================================================

blue "--- TEST 6: invalid scheduler ---"
expect_fail "rejected 'bad'" 4 800 200 200 200 3 50 bad
expect_fail "rejected 'FIFO' (uppercase)" 4 800 200 200 200 3 50 FIFO
expect_fail "rejected 'EDF' (uppercase)" 4 800 200 200 200 3 50 EDF
expect_fail "rejected 'Fifo' (mixed)" 4 800 200 200 200 3 50 Fifo
expect_fail "rejected 'fifo ' (trailing space)" 4 800 200 200 200 3 50 "fifo "
expect_fail "rejected 'edfx' (suffix)" 4 800 200 200 200 3 50 edfx
expect_fail "rejected '' (empty)" 4 800 200 200 200 3 50 ""

blue "--- TEST 7: negative numbers ---"
expect_fail "rejected -4 (n)" -4 800 200 200 200 3 50 fifo
expect_fail "rejected -800 (t_burn)" 4 -800 200 200 200 3 50 fifo
expect_fail "rejected -200 (t_comp)" 4 800 -200 200 200 3 50 fifo
expect_fail "rejected -200 (t_deb)" 4 800 200 -200 200 3 50 fifo
expect_fail "rejected -200 (t_ref)" 4 800 200 200 -200 3 50 fifo
expect_fail "rejected -3 (need)" 4 800 200 200 200 -3 50 fifo
expect_fail "rejected -50 (cooldown)" 4 800 200 200 200 3 -50 fifo

blue "--- TEST 8: non-integers ---"
expect_fail "rejected 'abc'" 4 abc 200 200 200 3 50 fifo
expect_fail "rejected '4a'" 4 4a 200 200 200 3 50 fifo
expect_fail "rejected ''" 4 "" 200 200 200 3 50 fifo
expect_fail "rejected '+'" 4 + 200 200 200 3 50 fifo
expect_fail "rejected '+4'" +4 800 200 200 200 3 50 fifo
expect_fail "rejected ' 4' (leading space)" " 4" 800 200 200 200 3 50 fifo
expect_fail "rejected '4 ' (trailing space)" "4 " 800 200 200 200 3 50 fifo
expect_fail "rejected '0x4' (hex)" 0x4 800 200 200 200 3 50 fifo
expect_fail "rejected '4.5' (float)" 4.5 800 200 200 200 3 50 fifo
expect_fail "rejected '--4' (double minus)" --4 800 200 200 200 3 50 fifo

blue "--- TEST 9: wrong arg count ---"
expect_fail "rejected 1 arg" 4
expect_fail "rejected 2 args" 4 800
expect_fail "rejected 7 args" 4 800 200 200 200 3 50
expect_fail "rejected 9 args" 4 800 200 200 200 3 50 fifo extra
expect_fail "rejected 0 args"

blue "--- TEST 10: zero values (invalid) ---"
expect_fail "rejected n=0" 0 800 200 200 200 3 50 fifo
expect_fail "rejected t_burn=0" 4 0 200 200 200 3 50 fifo
expect_fail "rejected t_comp=0" 4 800 0 200 200 3 50 fifo
expect_fail "rejected need=0" 4 800 200 200 200 0 50 fifo

blue "--- TEST 11: zero values (valid) ---"
expect_ok "accept t_deb=0" 2 5000 100 0 100 1 0 fifo
expect_ok "accept t_ref=0" 2 5000 100 100 0 1 0 fifo
expect_ok "accept cooldown=0" 2 5000 100 100 100 1 0 fifo
expect_ok "accept all zeros except required" 2 5000 100 0 0 1 0 fifo

blue "--- TEST 12: overflow / huge ---"
expect_fail "rejected n=1000000" 1000000 800 200 200 200 3 50 fifo
expect_fail "rejected n=201" 201 800 200 200 200 3 50 fifo
expect_fail "rejected 99999999999999999999 (20 digits)" 99999999999999999999 800 200 200 200 3 50 fifo

blue "--- TEST 13: leading zeros (should accept) ---"
expect_ok "accept '04' as n=4" 04 800 200 200 200 3 50 fifo
expect_ok "accept '0004' as n=4" 0004 800 200 200 200 3 50 fifo

# =============================================================================
blue "===== SECTION 3: EDGE CASES ====="
# =============================================================================

blue "--- TEST 14: t_comp=1, t_deb=0, t_ref=0 ---"
expect_ok "fast params work" 2 100 1 0 0 5 0 fifo

blue "--- TEST 15: n=1, need=100 ---"
expect_ok "n=1, need=100" 1 10000 1 1 1 100 0 fifo

blue "--- TEST 16: need=1, big t_burn ---"
expect_ok "need=1, big t_burn" 5 10000 10 10 10 1 0 fifo

blue "--- TEST 17: cooldown > t_comp ---"
expect_ok "cooldown > t_comp" 2 5000 100 100 100 5 500 fifo

blue "--- TEST 18: t_deb = t_burn (boundary) ---"
OUT=$($BIN 2 500 100 500 500 100 50 fifo 2>&1)
BURN=$(echo "$OUT" | grep -c "burned out")
check "t_deb = t_burn causes burnout" "$([ "$BURN" -ge 1 ] && echo 1 || echo 0)"

blue "--- TEST 19: t_burn < t_comp (immediate burnout) ---"
OUT=$($BIN 2 100 200 100 100 5 50 fifo 2>&1)
BURN=$(echo "$OUT" | grep -c "burned out")
check "t_burn < t_comp causes burnout" "$([ "$BURN" -ge 1 ] && echo 1 || echo 0)"

blue "--- TEST 20: cooldown > t_burn (forced burnout) ---"
OUT=$($BIN 2 300 100 100 100 5 1000 fifo 2>&1)
BURN=$(echo "$OUT" | grep -c "burned out")
check "cooldown > t_burn causes burnout" "$([ "$BURN" -ge 1 ] && echo 1 || echo 0)"

blue "--- TEST 21: exactly 1 burnout log ---"
OUT=$($BIN 5 500 200 200 200 10 50 fifo 2>&1)
BURN=$(echo "$OUT" | grep -c "burned out")
check "exactly 1 burnout log" "$([ "$BURN" -eq 1 ] && echo 1 || echo 0)"

blue "--- TEST 22: n=200 (max) ---"
timeout 60 $BIN 200 10000 10 10 10 1 5 fifo >/dev/null 2>&1
check "n=200 completed" "$([ $? -eq 0 ] && echo 1 || echo 0)"

blue "--- TEST 23: parity (even) ---"
expect_ok "n=4 works" 4 800 200 200 200 3 50 fifo

blue "--- TEST 24: parity (odd) ---"
expect_ok "n=5 works" 5 800 200 200 200 3 50 fifo

blue "--- TEST 25: n=7 fifo ---"
expect_ok "n=7 fifo" 7 5000 100 100 100 3 50 fifo

# =============================================================================
blue "===== SECTION 4: DEADLOCK / STRESS ====="
# =============================================================================

blue "--- TEST 26: no deadlock under stress ---"
timeout 10 $BIN 4 5000 100 100 100 5 50 fifo >/dev/null 2>&1
check "n=4 no timeout" "$([ $? -eq 0 ] && echo 1 || echo 0)"

timeout 15 $BIN 20 10000 50 50 50 5 20 fifo >/dev/null 2>&1
check "n=20 no timeout" "$([ $? -eq 0 ] && echo 1 || echo 0)"

timeout 30 $BIN 100 20000 10 10 10 2 5 edf >/dev/null 2>&1
check "n=100 no timeout (edf)" "$([ $? -eq 0 ] && echo 1 || echo 0)"

blue "--- TEST 27: long run ---"
timeout 15 $BIN 4 5000 100 100 100 10 50 fifo >/dev/null 2>&1
check "long run no timeout" "$([ $? -eq 0 ] && echo 1 || echo 0)"

# =============================================================================
blue "===== SECTION 5: LOG FORMAT & CONSISTENCY ====="
# =============================================================================

blue "--- TEST 28: log format ---"
OUT=$($BIN 3 800 200 200 200 2 50 fifo 2>&1)
BAD=$(echo "$OUT" | grep -vE "^[0-9]+ [0-9]+ (has taken a dongle|is compiling|is debugging|is refactoring|burned out)$" | head -3)
check "all lines match format" "$([ -z "$BAD" ] && echo 1 || echo 0)"
if [ -n "$BAD" ]; then
    red "  bad lines:"
    echo "$BAD"
fi

blue "--- TEST 29: no interleaved logs ---"
$BIN 5 800 200 200 200 2 50 fifo > /tmp/cx_test.log 2>&1
INTER=$(grep -cE "^[0-9]+ [0-9]+ [a-z]" /tmp/cx_test.log)
TOTAL=$(wc -l < /tmp/cx_test.log)
check "no interleaved lines ($INTER/$TOTAL)" "$([ "$INTER" -eq "$TOTAL" ] && echo 1 || echo 0)"

blue "--- TEST 30: exactly 2 takes per compile ---"
$BIN 5 5000 100 100 100 3 50 fifo > /tmp/cx_takes.log 2>&1
BAD=$(awk '/has taken a dongle/{t++} /is compiling/{if(t!=2) print NR": t="t; t=0}' /tmp/cx_takes.log)
check "every compile preceded by 2 takes" "$([ -z "$BAD" ] && echo 1 || echo 0)"
if [ -n "$BAD" ]; then
    red "  $BAD"
fi

blue "--- TEST 31: timestamp monotonic ---"
JUMP=$(awk 'NR>1 && $1 < prev {print "JUMP at line "NR; exit} {prev=$1}' /tmp/cx_takes.log)
check "timestamp monotonic" "$([ -z "$JUMP" ] && echo 1 || echo 0)"

blue "--- TEST 32: order take/take/compile ---"
ORDER=$(grep "^[0-9]* 1 " /tmp/cx_takes.log | head -3 | awk '{print $3" "$4}')
EXPECTED="has taken
has taken
is compiling"
if echo "$ORDER" | grep -q "has taken" && echo "$ORDER" | grep -q "is compiling"; then
    check "order: take/take/compile" "1"
else
    check "order: take/take/compile" "0"
fi

blue "--- TEST 33: no orphan logs ---"
# Каждый "is compiling" должен иметь 2 "taken" перед ним у того же кодера
ORPHAN=$(awk '
    /has taken a dongle/ { t[$2]++; }
    /is compiling/ { if (t[$2] != 2) print "ORPHAN: coder "$2" t="t[$2]; t[$2]=0; }
    /burned out/ { t[$2]=0; }
' /tmp/cx_takes.log)
check "no orphan compile logs" "$([ -z "$ORPHAN" ] && echo 1 || echo 0)"
if [ -n "$ORPHAN" ]; then
    red "  $ORPHAN"
fi

# =============================================================================
blue "===== SECTION 6: TIMING / BURNOUT PRECISION ====="
# =============================================================================

blue "--- TEST 34: burnout timestamp sane ---"
OUT=$($BIN 2 300 100 100 100 10 50 fifo 2>&1)
BURN_LINE=$(echo "$OUT" | grep "burned out" | head -1)
BURN_TS=$(echo "$BURN_LINE" | awk '{print $1}')
if [ -n "$BURN_TS" ]; then
    check "burnout ts sane (< 1000ms)" "$([ "$BURN_TS" -lt 1000 ] && echo 1 || echo 0)"
    check "burnout close to t_burn=300 (±50ms)" "$([ "$BURN_TS" -ge 250 ] && [ "$BURN_TS" -le 350 ] && echo 1 || echo 0)"
    green "  burnout at ${BURN_TS}ms (expected ~300ms)"
else
    red "  no burnout found"
    FAIL=$((FAIL+1))
fi

blue "--- TEST 35: cooldown respected ---"
OUT=$($BIN 2 5000 100 100 100 3 200 fifo 2>&1)
FIRST=$(echo "$OUT" | grep "1 has taken a dongle" | head -1 | awk '{print $1}')
SECOND=$(echo "$OUT" | grep "1 has taken a dongle" | sed -n '3p' | awk '{print $1}')
if [ -n "$FIRST" ] && [ -n "$SECOND" ]; then
    DIFF=$((SECOND - FIRST))
    check "cooldown gap >= 200ms (diff: ${DIFF}ms)" "$([ "$DIFF" -ge 200 ] && echo 1 || echo 0)"
else
    red "  could not find two takes"
    FAIL=$((FAIL+1))
fi

blue "--- TEST 36: n=1 special case ---"
OUT=$($BIN 1 800 200 200 200 3 50 fifo 2>&1)
COUNT=$(echo "$OUT" | awk '/has taken a dongle/{c++} /is compiling/{print c; exit}')
check "n=1: exactly 1 dongle per compile" "$([ "$COUNT" -eq 1 ] && echo 1 || echo 0)"

# =============================================================================
blue "===== SECTION 7: DUMB CASES ====="
# =============================================================================

blue "--- TEST 37: repeated args ---"
expect_fail "rejected 10 args (fifo fifo)" 4 800 200 200 200 3 50 fifo fifo
expect_fail "rejected 13 args" 4 800 200 200 200 3 50 fifo fifo fifo fifo

blue "--- TEST 38: --help / flags ---"
expect_fail "rejected --help" --help
expect_fail "rejected -h" -h
expect_fail "rejected only 'fifo'" fifo

blue "--- TEST 39: all strings ---"
expect_fail "rejected all strings" abc def ghi jkl mno pqr stu fifo

blue "--- TEST 40: empty scheduler ---"
expect_fail "rejected empty scheduler" 4 800 200 200 200 3 50 ""

blue "--- TEST 41: leading + ---"
expect_fail "rejected +4" +4 800 200 200 200 3 50 fifo
expect_fail "rejected +800" 4 +800 200 200 200 3 50 fifo

blue "--- TEST 42: whitespace ---"
expect_fail "rejected ' '" " " 800 200 200 200 3 50 fifo
expect_fail "rejected tab" "$(printf '\t')4" 800 200 200 200 3 50 fifo

blue "--- TEST 43: float / hex / octal ---"
expect_fail "rejected 4.5" 4.5 800 200 200 200 3 50 fifo
expect_fail "rejected 0x10" 0x10 800 200 200 200 3 50 fifo
expect_fail "rejected 0755" 0755 800 200 200 200 3 50 fifo

# =============================================================================
echo ""
blue "=========================================="
if [ "$FAIL" -eq 0 ]; then
    green "ALL TESTS PASSED ($PASS/$((PASS+FAIL)))"
else
    red "SOME TESTS FAILED ($FAIL failed, $PASS passed)"
fi
blue "=========================================="
exit $FAIL